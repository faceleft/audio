project('audio', 'cpp')

portaudio_dep = dependency('portaudiocpp')
rnnoise_dep = dependency('rnnoise')

audio = static_library(
  'audio',
  [
    'src/lib.cpp',
    'src/player.cpp',
    'src/recorder.cpp',
    'src/rnnoise_dsp.cpp',
    'src/volume_dsp.cpp',
  ],
  dependencies: [portaudio_dep, rnnoise_dep],
  include_directories: ['include/'],
)

audio_dep = declare_dependency(link_with: audio, include_directories: 'include/')

executable('echo_example', 'examples/echo.cpp', dependencies: audio_dep)