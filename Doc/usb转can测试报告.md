# 测试环境

CAN分析仪，周立功ZCANPRO上位机

**DDT Link**

达妙usb转can上位机

串口助手

# 测试结果

## 达妙上位机发送反馈

设置达妙usb转can上位机参数如下：

<img src="Image/image-20240823150456005.png" alt="image-20240823150456005" style="zoom: 50%;" />

达妙usb转can上位机发送完成页面：

<img src="Image/image-20240823150532798.png" alt="image-20240823150532798" style="zoom:67%;" />

周立功上位机接收结果：

<img src="Image/image-20240823150603847.png" alt="image-20240823150603847" style="zoom:67%;" />

可以看到通信正常

## 达妙上位机接收反馈

使用周立功上位机发送数据：

<img src="Image/image-20240823151832178.png" alt="image-20240823151832178" style="zoom: 67%;" />

达妙上位机页面：

![image-20240823151918918](Image/image-20240823151918918.png)

可以看到通信正常

# 通信速率

## 使能达妙上位机反馈功能

因为达妙usb转can上位机不支持多次连续发送，使用串口助手测试

串口助手参数如下：

![image-20240823151206110](Image/image-20240823151206110.png)

周立功上位机接收结果：

<img src="Image/image-20240823151113427.png" alt="image-20240823151113427" style="zoom:67%;" />

接收9571帧，耗时23.0194s

平均每帧耗时2.4ms

## 不使能达妙上位机反馈功能

<img src="Image/image-20240823151553199.png" alt="image-20240823151553199" style="zoom:67%;" />

接收9725帧，耗时14.089s

平均每帧耗时1.4ms

