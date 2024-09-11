#!/bin/bash
# git clone -b xyzw6789abcd1234efgh5678ijkl9012mnop --single-branch --branch <branch_name> <GitHub_Repo_URL>

git_clone_and_checkout_commit()
{
    git clone $1
    url=$1
    repo_name="${url##*/}"
    repo_name="${repo_name%.git}"
    pushd $repo_name
    git checkout $2
    popd
}

git_clone_and_checkout_commit https://github.com/dianjixz/mongoose.git      dd5118cab0813078dbbfd116f1489c6a03a42cae
git_clone_and_checkout_commit https://github.com/dianjixz/libhv.git         1ab8a8e551a5ab668988549779578553e3c3aa1e
# git_clone_and_checkout_commit https://github.com/ithewei/libhv.git         c4e3b536fee8c2bfa67eec70eedf5ea0f0a4f60f
git_clone_and_checkout_commit https://github.com/dianjixz/RtspServer.git    905357880a73b5e03fb2adb17b7df10eadcd5e4a
git_clone_and_checkout_commit https://github.com/dianjixz/xop.git           b8c034d72a7d879e92ec3cbc2082481c78205cd1
git_clone_and_checkout_commit https://github.com/dianjixz/gear-lib.git      cf68fddccefafaa4a5b05082819f66d8419f5811
# git_clone_and_checkout_commit https://github.com/dianjixz/stb.git           5736b15f7ea0ffb08dd38af21067c314d6a3aae9
git_clone_and_checkout_commit https://github.com/dianjixz/stb.git           3b31d1d0a3fd0e563fd5cfaabf67c9921a08bb61
git_clone_and_checkout_commit https://github.com/dianjixz/libs.git          a6456b8e7f1564b0c94c329509fe9652bb5a096b
git_clone_and_checkout_commit https://github.com/dianjixz/nanomsg.git       2d01e935e4554580da4e9dbd94964af94e0ded9c
# cd lvgl && git_clone_and_checkout_commit https://github.com/dianjixz/lvgl.git          5cdf242a21a0897ee1f24b85040d2993a817b822
# cd lvgl && git_clone_and_checkout_commit https://github.com/lvgl/lv_drivers.git        820341ea15d0e9f0faaf0b508ba03c3ef9cb1a88
git_clone_and_checkout_commit https://github.com/dianjixz/toolkit.git       64120e079446c77990d892865fa5a105fc46036f
git_clone_and_checkout_commit https://github.com/dianjixz/tbox.git          2e0cac8bae7a8bc921114b1e482fefd8f8a5d55e
git_clone_and_checkout_commit https://github.com/wuhanstudio/u8g2-arm-linux.git 31a02e6e22ece76fe9bca45d472ed28c4dea720b
# git_clone_and_checkout_commit https://github.com/dianjixz/M5GFX.git         7370acbb2379b54f21ffeba70d72da9dbd5ec801
git_clone_and_checkout_commit https://github.com/dianjixz/minicv2.git       e265532942671c34b4d502bfe47fadfca73b7730
git_clone_and_checkout_commit https://github.com/bombela/backward-cpp.git   65a769ffe77cf9d759d801bc792ac56af8e911a3
git_clone_and_checkout_commit https://github.com/OpenEtherCATsociety/SOEM.git a901500618405760a564e64a6816705e29f50f9f
git_clone_and_checkout_commit https://github.com/dianjixz/c-periphery.git   8e69e684e7b04b36cb7ab795d01453df7b4a9c65

git_clone_and_checkout_commit https://github.com/brgl/libgpiod.git   27fe10150f6d5fa78d1a1ef1e922dc8395d1154d

git_clone_and_checkout_commit https://github.com/stephane/libmodbus.git   5c14f13944ec7394a61a7680dcb462056c4321e3
git_clone_and_checkout_commit https://github.com/lovyan03/LovyanGFX.git   4fd713ebd3019d17c6c89c6e2f7171d2cafeff23
git_clone_and_checkout_commit https://github.com/Tencent/ncnn.git   5e9bf90455b02a25e7991dab4ab8b3035a4bebb3
git_clone_and_checkout_commit https://github.com/libtom/libtommath.git   97e500351e653472b2ea00b37137ad0414165efe
git_clone_and_checkout_commit https://github.com/talybin/firebird.git   16bb82d461bbb0bae10390adc20fe75ef4f420d4
git_clone_and_checkout_commit https://github.com/Smalldy/EasyMsgCpp.git   a265a4049eddd844a19c74850e4e6e53ca007a7f
git_clone_and_checkout_commit https://github.com/fukuchi/libqrencode.git   715e29fd4cd71b6e452ae0f4e36d917b43122ce8
git_clone_and_checkout_commit https://github.com/tinyalsa/tinyalsa.git   7f06b2f219294de385caf9bcbca24b0709224f8c
git_clone_and_checkout_commit https://github.com/libjpeg-turbo/libjpeg-turbo.git 8db0312668f986891f65af9dbcc94e7e92ede099

# opencv-mobile-2.4.13.7-armlinux https://github.com/nihui/opencv-mobile/releases/download/v26/opencv-mobile-2.4.13.7-armlinux.zip

# git clone -b dd5118cab0813078dbbfd116f1489c6a03a42cae --single-branch --branch master https://github.com/dianjixz/mongoose.git
# git clone -b 1ab8a8e551a5ab668988549779578553e3c3aa1e --single-branch --branch master https://github.com/dianjixz/libhv.git
# git clone -b 905357880a73b5e03fb2adb17b7df10eadcd5e4a --single-branch --branch master https://github.com/dianjixz/RtspServer.git
# git clone -b b8c034d72a7d879e92ec3cbc2082481c78205cd1 --single-branch --branch master https://github.com/dianjixz/xop.git
# git clone -b cf68fddccefafaa4a5b05082819f66d8419f5811 --single-branch --branch my_test https://github.com/dianjixz/gear-lib.git
# git clone -b 5736b15f7ea0ffb08dd38af21067c314d6a3aae9 --single-branch --branch master https://github.com/dianjixz/stb.git
# git clone -b a6456b8e7f1564b0c94c329509fe9652bb5a096b --single-branch --branch main https://github.com/dianjixz/libs.git
# git clone -b 2d01e935e4554580da4e9dbd94964af94e0ded9c --single-branch --branch master https://github.com/dianjixz/nanomsg.git
# git clone -b c5e9d8acfc226418dedcf2e34a617bffae043ff6 --single-branch --branch master https://github.com/dianjixz/nng.git
# cd lvgl && git clone -b  5cdf242a21a0897ee1f24b85040d2993a817b822 --single-branch --branch master https://github.com/dianjixz/lvgl.git
# cd lvgl && git clone -b  820341ea15d0e9f0faaf0b508ba03c3ef9cb1a88 --single-branch --branch master https://github.com/lvgl/lv_drivers.git
# git clone -b  64120e079446c77990d892865fa5a105fc46036f --single-branch --branch master https://github.com/dianjixz/toolkit.git
# git clone -b  2e0cac8bae7a8bc921114b1e482fefd8f8a5d55e --single-branch --branch master https://github.com/dianjixz/tbox.git
# git clone -b  31a02e6e22ece76fe9bca45d472ed28c4dea720b --single-branch --branch master https://github.com/wuhanstudio/u8g2-arm-linux.git
# git clone -b  7370acbb2379b54f21ffeba70d72da9dbd5ec801 --single-branch --branch master https://github.com/dianjixz/M5GFX.git
# git clone -b  e265532942671c34b4d502bfe47fadfca73b7730 --single-branch --branch master https://github.com:dianjixz/minicv2.git
# git clone -b  65a769ffe77cf9d759d801bc792ac56af8e911a3 --single-branch --branch master https://github.com/bombela/backward-cpp.git
# git clone -b  a901500618405760a564e64a6816705e29f50f9f --single-branch --branch master https://github.com/OpenEtherCATsociety/SOEM.git
# git clone -b  ec31b393abd63620d555cd99cdc9d1b77d88887b --single-branch --branch master https://github.com/dianjixz/c-periphery.git

