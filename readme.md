
## 转载请注明出处：
@github地址:git@github.com:mhming/offlineros2demo.git

@简书地址：[科大讯飞麦克风阵列模块ROS2离线使用记录](https://www.jianshu.com/p/ddf8daed5ea6)
## 本代码基于轮趣科技提供的离线demo修改
@轮趣科技：[远场麦克风阵列语音板ROS六麦模块语音线性4麦6麦科大讯飞降噪](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.52f62e8dIJHJi1&id=633822976782&_u=1ej2pvta40b)

# 使用说明
1. 按照官方要求安装相关驱动
2. 安装ROS2, 本文测试使用ROS2-Foxy
3. 将本代码下载到本地ROS2工作空间
4. 修改科大讯飞的注册试用APPID
位于：asr_offline_record_sample.cpp的7~9行
5. 下载自己的离线语音识别配置文件，并替换configs目录下的common.jet文件为自己下载的
6. 修改语法相关参数,路径最好设置为绝对地址
位于：asr_offline_record_sample.cpp的9~11行
7. 使用"arecord -l" 命令查看自己的设备编号并修改到src->record.h的第15行
8. 编译：colcon build --packages-select offlineros2demo
9. source当前工作空间：source install/setup.bash
10. 运行测试：ros2 run offlineros2demo offlineros2demo
