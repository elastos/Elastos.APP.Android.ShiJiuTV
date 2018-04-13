环境准备：
  主机：Ubuntu x86_64
  ./src/build/install-build-deps.sh

编译：
  cd Elastos.APP.Android.ShiJiuTV
  git checkout develop && git reset --hard HEAD #仅第一次执行时需要

  export PATH=$PATH:$PWD/depot_tools;
  cd src
  gn gen --args='target_os="android"' out/Default #仅第一次执行或out目录被删除时需要
  ninja -C out/Default chrome_public_apk -j16

Demo:
  用adb链接设备。
  adb push ../demo/libElastos.HelloCarDemo.so /data/local/tmp/

  adb install -r out/Default/apks/ChromePublic.apk

  将文件../demo/libElastos.HelloCarDemo.html放入一个server，如：http://192.168.2.99/libElastos.HelloCarDemo.html
  启动Chromium, 在网址中填入http://192.168.2.99/libElastos.HelloCarDemo.html
