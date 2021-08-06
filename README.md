# Demo2Plus
The real PBR with IBL
PBR光照实现： 如果需要运行请切换至x86编译， 使用opengl4.6标准编写，如果遇到fail to create GLFW window请检查opengl版本

在场景中wasd移动，鼠标右键加速，x键退出程序。 Image text 实现了基于Cook-Torrance BRDF的PBR光照模型
，可以通过调整菲涅尔系数freh和粗糙度rough来模拟各种材质

使用IBL来模拟间接光照，载入不同的环境贴图时会自动预计算相应的辐照度贴图，BRDF贴图，和镜面反射贴图。
可以通过更换不同的贴图和给材质球加上材质贴图来产生各种各样令人惊异的效果。哪怕是后图中的Bunny低模，甚至在没有法线的情况下依然达到了十分真实的渲染效果

内置了法线贴图和次级法线贴图，分离了Shader和主逻辑代码，可以自行决定是否在Shader里启用。增加了帧缓冲，可以用来实现粒子模糊/径向模糊/景深/Bloom等后处理效果。相应的实现同样可以选择启用与否。支持模型的热更新。

支持Mask的启用，Mask贴图数量受限于OpenGL支持的最大纹理数-其他纹理数。但是同样可以使用内置的工具将Mask存在顶点信息中。
具有良好的性能，比在Unity上相同的实现快了30%-50%。

部分效果预览：
![Image text](https://github.com/decsacety/Demo2Plus/blob/main/OpenGL01/source/1.png)
![Image text](https://github.com/decsacety/Demo2Plus/blob/main/OpenGL01/source/2.png)
![Image text](https://github.com/decsacety/Demo2Plus/blob/main/OpenGL01/source/3.png)
![Image text](https://github.com/decsacety/Demo2Plus/blob/main/OpenGL01/source/4.png)
![Image text](https://github.com/decsacety/Demo2Plus/blob/main/OpenGL01/source/5.png)
![Image text](https://github.com/decsacety/Demo2Plus/blob/main/OpenGL01/source/6.png)
