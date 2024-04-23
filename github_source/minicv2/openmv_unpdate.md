# openmv更新追踪

- 2022年2月8号更新记录
更新log
~~~ bash
更新 be2aad13..9eb48bae
Fast-forward
 .github/workflows/firmware.yml                       |  128 +++++
 .travis.yml                                          |  150 ------
 scripts/libraries/rpc.py                             |   15 +-
 src/Makefile                                         |    2 +
 src/lib/libtf/Makefile                               |   32 ++
 src/lib/libtf/cortex-m0plus/LICENSE                  |  248 +++++++++
 src/lib/libtf/cortex-m0plus/README                   |    1 +
 src/lib/libtf/cortex-m0plus/libtf.a                  |  Bin 0 -> 1095954 bytes
 src/lib/libtf/cortex-m4/LICENSE                      |   45 ++
 src/lib/libtf/cortex-m55/LICENSE                     |   45 ++
 src/lib/libtf/cortex-m55/libtf.h                     |   83 ---
 src/lib/libtf/cortex-m7/LICENSE                      |   45 ++
 src/lib/libtf/cortex-m7/libtf.h                      |   83 ---
 src/lib/libtf/{cortex-m4 => }/libtf.h                |    4 -
 src/lib/libtf/models/person_detection.tflite         |  Bin 0 -> 300568 bytes
 src/lib/libtf/models/person_detection.txt            |    2 +
 src/micropython                                      |    2 +-
 src/omv/Makefile                                     |    2 +
 src/omv/boards/{BORMIO => NICLAV}/imlib_config.h     |    8 +-
 src/omv/boards/{BORMIO => NICLAV}/manifest.py        |    0
 src/omv/boards/{BORMIO => NICLAV}/omv_boardconfig.h  |    4 +-
 src/omv/boards/{BORMIO => NICLAV}/omv_boardconfig.mk |    0
 src/omv/boards/NICLAV/readme_txt.h                   |   19 +
 src/omv/boards/{BORMIO => NICLAV}/ulab_config.h      |    0
 src/omv/boards/OPENMV3/imlib_config.h                |    6 +-
 src/omv/boards/OPENMV4/imlib_config.h                |    6 +-
 src/omv/boards/OPENMV4P/imlib_config.h               |    6 +-
 src/omv/boards/OPENMVPT/imlib_config.h               |    6 +-
 src/omv/boards/PORTENTA/imlib_config.h               |    4 +
 src/omv/common/ini.c                                 |    2 +-
 src/omv/imlib/draw.c                                 |   14 +-
 src/omv/imlib/framebuffer.c                          |    4 +-
 src/omv/imlib/imlib.c                                |   33 +-
 src/omv/imlib/imlib.h                                |   35 +-
 src/omv/imlib/jpeg.c                                 |   10 +
 src/omv/imlib/lodepng.c                              | 6503 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 src/omv/imlib/lodepng.h                              | 2026 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 src/omv/imlib/png.c                                  |  337 ++++++++++++
 src/omv/modules/py_image.c                           |  256 ++++-----
 src/omv/modules/py_tf.c                              |   62 ++-
 src/omv/ports/nrf/omv_portconfig.mk                  |    2 +
 src/omv/ports/rp2/omv_portconfig.cmake               |    2 +
 src/omv/ports/stm32/omv_portconfig.mk                |   46 +-
 tools/ci.sh                                          |   51 ++
 tools/tflite2c.py                                    |   77 +++
 45 files changed, 9864 insertions(+), 542 deletions(-)
 create mode 100644 .github/workflows/firmware.yml
 delete mode 100644 .travis.yml
 create mode 100644 src/lib/libtf/Makefile
 create mode 100644 src/lib/libtf/cortex-m0plus/LICENSE
 create mode 100644 src/lib/libtf/cortex-m0plus/README
 create mode 100644 src/lib/libtf/cortex-m0plus/libtf.a
 delete mode 100644 src/lib/libtf/cortex-m55/libtf.h
 delete mode 100644 src/lib/libtf/cortex-m7/libtf.h
 rename src/lib/libtf/{cortex-m4 => }/libtf.h (96%)
 create mode 100644 src/lib/libtf/models/person_detection.tflite
 create mode 100644 src/lib/libtf/models/person_detection.txt
 rename src/omv/boards/{BORMIO => NICLAV}/imlib_config.h (94%)
 rename src/omv/boards/{BORMIO => NICLAV}/manifest.py (100%)
 rename src/omv/boards/{BORMIO => NICLAV}/omv_boardconfig.h (99%)
 rename src/omv/boards/{BORMIO => NICLAV}/omv_boardconfig.mk (100%)
 create mode 100644 src/omv/boards/NICLAV/readme_txt.h
 rename src/omv/boards/{BORMIO => NICLAV}/ulab_config.h (100%)
 create mode 100644 src/omv/imlib/lodepng.c
 create mode 100644 src/omv/imlib/lodepng.h
 create mode 100644 src/omv/imlib/png.c
 create mode 100755 tools/ci.sh
 create mode 100755 tools/tflite2c.py
~~~
添加了 png 图片格式的支持
等待合并到 minicv2 中！ 