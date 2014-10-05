/*
    Copyright (C) 2012 Stefan Hajnoczi <stefanha@gmail.com>

    Wahjam is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Wahjam is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wahjam; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <portaudio.h>
#include <QtGlobal>
#include "audiostream.h"

static void logPortAudioError(const char *msg, PaError error)
{
  if (error == paUnanticipatedHostError) {
    const PaHostErrorInfo *info = Pa_GetLastHostErrorInfo();
    qCritical("%s: unanticipated host error: %s (%ld)",
              msg, info->errorText, info->errorCode);
  } else {
    qCritical("%s: %s", msg, Pa_GetErrorText(error));
  }
}

class PortAudioStreamer : public audioStreamer
{
public:
  PortAudioStreamer(SPLPROC proc);
  ~PortAudioStreamer();

  bool Start(const PaStreamParameters *inputParams,
             const PaStreamParameters *outputParams,
             double sampleRate);
  void Stop();

  const char *GetChannelName(int idx);

  int streamCallback(const void *input, void *output,
    unsigned long frameCount, const PaStreamCallbackTimeInfo *timeInfo,
    PaStreamCallbackFlags statusFlags);

  static int streamCallbackTrampoline(const void *input, void *output,
      unsigned long frameCount, const PaStreamCallbackTimeInfo *timeInfo,
      PaStreamCallbackFlags statusFlags, void *userData);

private:
  SPLPROC splproc;
  PaStream *stream;
  float *inputMonoBuf;
  unsigned long inputMonoBufFrames;
};

const char *PortAudioStreamer::GetChannelName(int idx)
{
  if (idx >= m_innch) {
    return NULL;
  }

  /* TODO make GetChannelName() reentrancy-safe */
  static char name[64];
  snprintf(name, sizeof name, "Channel %d", idx);
  return name;
}

int PortAudioStreamer::streamCallback(const void *input, void *output,
    unsigned long frameCount, const PaStreamCallbackTimeInfo *timeInfo,
    PaStreamCallbackFlags statusFlags)
{
  Q_UNUSED(timeInfo);
  Q_UNUSED(statusFlags);

  float **inbuf = (float**)input; // const-cast due to SPLPROC prototype
  float **outbuf = static_cast<float**>(output);
  const PaStreamInfo *info = Pa_GetStreamInfo(stream);

  /* Mix down to mono */
  if (m_innch == 2) {
    if (inputMonoBufFrames < frameCount) {
      /* Allocation should happen rarely so don't worry about real-time */
      delete [] inputMonoBuf;
      inputMonoBuf = new float[frameCount];
      inputMonoBufFrames = frameCount;
    }
    for (unsigned long i = 0; i < frameCount; i++) {
      inputMonoBuf[i] = inbuf[0][i] * 0.5 + inbuf[1][i] * 0.5;
    }
    inbuf = &inputMonoBuf;
  }

  splproc(inbuf, 1, outbuf, 1, frameCount, info->sampleRate);

  /* Mix up to stereo */
  if (m_outnch == 2) {
    for (unsigned long i = 0; i < frameCount; i++) {
      outbuf[0][i] = outbuf[1][i] = outbuf[0][i] * 0.5;
    }
  }

  return paContinue;
}

int PortAudioStreamer::streamCallbackTrampoline(const void *input, void *output,
    unsigned long frameCount, const PaStreamCallbackTimeInfo *timeInfo,
    PaStreamCallbackFlags statusFlags, void *userData)
{
  PortAudioStreamer *streamer = static_cast<PortAudioStreamer*>(userData);

  return streamer->streamCallback(input, output, frameCount, timeInfo, statusFlags);
}

PortAudioStreamer::PortAudioStreamer(SPLPROC proc)
  : splproc(proc), stream(NULL), inputMonoBuf(NULL), inputMonoBufFrames(0)
{
}

PortAudioStreamer::~PortAudioStreamer()
{
  Stop();
  delete [] inputMonoBuf;
}

/* Returns true on success, false on failure */
bool PortAudioStreamer::Start(const PaStreamParameters *inputParams,
                              const PaStreamParameters *outputParams,
                              double sampleRate)
{
  PaError error;

  qDebug("Trying Pa_OpenStream() with sampleRate %g inputLatency %g outputLatency %g innch %d outnch %d",
         sampleRate,
         inputParams->suggestedLatency,
         outputParams->suggestedLatency,
         inputParams->channelCount,
         outputParams->channelCount);
  const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(inputParams->device);
  if (deviceInfo) {
    const PaHostApiInfo *hostApiInfo = Pa_GetHostApiInfo(deviceInfo->hostApi);
    qDebug("Input device: %s (%s)", deviceInfo->name,
           hostApiInfo ? hostApiInfo->name : "<invalid host api>");
  }
  deviceInfo = Pa_GetDeviceInfo(outputParams->device);
  if (deviceInfo) {
    const PaHostApiInfo *hostApiInfo = Pa_GetHostApiInfo(deviceInfo->hostApi);
    qDebug("Output device: %s (%s)", deviceInfo->name,
           hostApiInfo ? hostApiInfo->name : "<invalid host api>");
  }

  error = Pa_OpenStream(&stream, inputParams, outputParams,
                        sampleRate, paFramesPerBufferUnspecified,
                        paPrimeOutputBuffersUsingStreamCallback,
                        streamCallbackTrampoline, this);
  if (error != paNoError) {
    logPortAudioError("Pa_OpenStream() failed", error);
    stream = NULL;
    return false;
  }

  m_srate = sampleRate;
  m_innch = inputParams->channelCount;
  m_outnch = outputParams->channelCount;
  m_bps = 32;

  error = Pa_StartStream(stream);
  if (error != paNoError) {
    logPortAudioError("Pa_StartStream() failed", error);
    Pa_CloseStream(stream);
    stream = NULL;
    return false;
  }

  const PaStreamInfo *streamInfo = Pa_GetStreamInfo(stream);
  if (streamInfo) {
    qDebug("Stream started with sampleRate %g inputLatency %g outputLatency %g",
           streamInfo->sampleRate,
           streamInfo->inputLatency,
           streamInfo->outputLatency);
  }

  return true;
}

void PortAudioStreamer::Stop()
{
  if (stream) {
    Pa_CloseStream(stream);
    stream = NULL;
  }
}

static const PaHostApiInfo *findHostAPIInfo(const char *hostAPI, PaHostApiIndex *index)
{
  const PaHostApiInfo *hostAPIInfo = NULL;
  PaHostApiIndex i;

  for (i = 0; i < Pa_GetHostApiCount(); i++) {
    hostAPIInfo = Pa_GetHostApiInfo(i);

    if (hostAPIInfo && strcmp(hostAPI, hostAPIInfo->name) == 0) {
      break;
    }
  }
  if (i >= Pa_GetHostApiCount()) {
    i = Pa_GetDefaultHostApi();
    hostAPIInfo = Pa_GetHostApiInfo(i);
  }

  if (index) {
    *index = i;
  }
  return hostAPIInfo;
}

static const PaDeviceInfo *findDeviceInfo(PaHostApiIndex hostAPI, const char *name,
                                          PaDeviceIndex *index, bool isInput)
{
  const PaHostApiInfo *hostAPIInfo = Pa_GetHostApiInfo(hostAPI);
  if (!hostAPIInfo) {
    return NULL;
  }

  const PaDeviceInfo *deviceInfo;
  PaDeviceIndex deviceIndex;
  int i;
  for (i = 0; i < hostAPIInfo->deviceCount; i++) {
    deviceIndex = Pa_HostApiDeviceIndexToDeviceIndex(hostAPI, i);
    if (deviceIndex < 0) {
      continue;
    }

    deviceInfo = Pa_GetDeviceInfo(deviceIndex);
    if (!deviceInfo) {
      continue;
    }
    if (isInput && deviceInfo->maxInputChannels == 0) {
      continue;
    }
    if (!isInput && deviceInfo->maxOutputChannels == 0) {
      continue;
    }
    if (strcmp(deviceInfo->name, name) == 0) {
      break;
    }
  }
  if (i >= hostAPIInfo->deviceCount) {
    deviceIndex = isInput ? hostAPIInfo->defaultInputDevice :
                            hostAPIInfo->defaultOutputDevice;
    deviceInfo = Pa_GetDeviceInfo(deviceIndex);
  }

  if (index) {
    *index = deviceIndex;
  }
  return deviceInfo;
}

static bool setupParameters(const char *hostAPI, const char *inputDevice,
    const char *outputDevice, PaStreamParameters *inputParams,
    PaStreamParameters *outputParams, double latency)
{
  PaHostApiIndex hostAPIIndex;
  if (!findHostAPIInfo(hostAPI, &hostAPIIndex)) {
    return false;
  }

  const PaDeviceInfo *inputDeviceInfo = findDeviceInfo(hostAPIIndex,
      inputDevice, &inputParams->device, true);
  if (!inputDeviceInfo) {
    return false;
  }

  const PaDeviceInfo *outputDeviceInfo = findDeviceInfo(hostAPIIndex,
      outputDevice, &outputParams->device, false);
  if (!outputDeviceInfo) {
    return false;
  }

  /* TODO do all host APIs/devices support this? */
  PaSampleFormat sampleFormat = paFloat32 | paNonInterleaved;
  inputParams->sampleFormat = sampleFormat;
  outputParams->sampleFormat = sampleFormat;

  /* TODO support user-defined channel configuration */
  inputParams->channelCount = inputDeviceInfo->maxInputChannels > 1 ? 2 : 1;
  outputParams->channelCount = outputDeviceInfo->maxOutputChannels > 1 ? 2 : 1;

  inputParams->suggestedLatency = latency;
  outputParams->suggestedLatency = latency;

  inputParams->hostApiSpecificStreamInfo = NULL;
  outputParams->hostApiSpecificStreamInfo = NULL;
  return true;
}

audioStreamer *create_audioStreamer_PortAudio(const char *hostAPI,
    const char *inputDevice, const char *outputDevice,
    double sampleRate, double latency, SPLPROC proc)
{
  PaStreamParameters inputParams, outputParams;
  if (!setupParameters(hostAPI, inputDevice, outputDevice,
                       &inputParams, &outputParams, latency)) {
    return NULL;
  }

  PortAudioStreamer *streamer = new PortAudioStreamer(proc);
  if (!streamer->Start(&inputParams, &outputParams, sampleRate)) {
    delete streamer;
    return NULL;
  }
  return streamer;
}

static void logPortAudioInfo()
{
  qDebug(Pa_GetVersionText());

  PaHostApiIndex api = 0;
  const PaHostApiInfo *apiInfo;
  while ((apiInfo = Pa_GetHostApiInfo(api))) {
    qDebug("Host API: %s (%d devices)", apiInfo->name, apiInfo->deviceCount);

    int device;
    for (device = 0; device < apiInfo->deviceCount; device++) {
      PaDeviceIndex devIdx = Pa_HostApiDeviceIndexToDeviceIndex(api, device);
      if (devIdx < 0) {
        qDebug("[%02d] Error: %s", device, Pa_GetErrorText(devIdx));
        continue;
      }

      const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(devIdx);
      if (!deviceInfo) {
        qDebug("[%02d] Invalid device index", device);
        continue;
      }

      qDebug("[%02d] \"%s\" (%d)", device, deviceInfo->name, devIdx);
      qDebug("     Channels: %d in, %d out",
          deviceInfo->maxInputChannels,
          deviceInfo->maxOutputChannels);
      qDebug("     Default sample rate: %g Hz",
          deviceInfo->defaultSampleRate);
      qDebug("     Input latency: %g low, %g high",
          deviceInfo->defaultLowInputLatency,
          deviceInfo->defaultHighInputLatency);
      qDebug("     Output latency: %g low, %g high",
          deviceInfo->defaultLowOutputLatency,
          deviceInfo->defaultHighOutputLatency);
    }
    api++;
  }
}

static void portAudioCleanup()
{
  Pa_Terminate();
}

/* Initialize PortAudio once for the whole application */
bool portAudioInit()
{
  PaError error = Pa_Initialize();
  if (error != paNoError) {
    logPortAudioError("Pa_Initialize() failed", error);
    return false;
  }
  atexit(portAudioCleanup);

  logPortAudioInfo();
  return true;
}
