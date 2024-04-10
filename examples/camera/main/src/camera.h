/* camera.h */
#ifndef CAMERA_H
#define CAMERA_H
 
#include <stdint.h>
 
typedef void (*vCameraFrameProcess)(uint8_t* pData, uint32_t Width, uint32_t Height, uint32_t Length) ;
 
/**
 *  打开摄像头
 * @pDevName 设备节点
 *  返回值: 0成功 -1失败
 */
int CameraOpen(const char* pDevName);
 
/**
 * 关闭摄像头
 * 返回值: 0成功 -1失败
 */
int CameraClose(void);
 
/**
 * 设置捕获帧回调
 * 返回值: 0成功 -1失败
*/
int CameraCaptureCallbackSet(vCameraFrameProcess pCallBack);
 
/**
 * 开始捕获
 * 返回值: 0成功 -1失败
*/
int CameraCaptureStart(void);
 
/**
 * 停止捕获
 * 返回值: 0成功 -1失败
*/
int CameraCaptureStop(void);
 
#endif