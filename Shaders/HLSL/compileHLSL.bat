echo Current directory is %cd%
C:/VulkanSDK/1.3.280.0/Bin/dxc.exe -T ps_6_0 -E main -spirv -fspv-target-env=vulkan1.0 -D VULKAN=1 -Fo frag.spv frag.hlsl
C:/VulkanSDK/1.3.280.0/Bin/dxc.exe -T vs_6_0 -E main -spirv -fspv-target-env=vulkan1.0 -D VULKAN=1 -Fo vert.spv vert.hlsl
C:/VulkanSDK/1.3.280.0/Bin/dxc.exe -T cs_6_0 -E main -spirv -fspv-target-env=vulkan1.0 -D VULKAN=1 -Fo particle.spv particle.hlsl
pause

