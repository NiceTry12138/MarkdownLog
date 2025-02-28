#pragma once

// 顶点信息 v0 版本 后续根据需要可能新增 v1、v2 ...
struct Vertex_v0
{
	float position[3];			  // 顶点坐标
	float color[4];				  // 顶点颜色
	float texCoord[2];			  // 顶点的 UV 坐标	
	int texIndex;				  // 顶点使用贴图的序号
};

