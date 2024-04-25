echo Current directory is %cd%
C:/VulkanSDK/1.3.280.0/Bin/dxc.exe -T ps_6_0 -E main -spirv -fvk-use-dx-layout -fspv-target-env=vulkan1.0 -D VULKAN=1 -Fo ../Compiled/HLSL/frag.spv frag.hlsl
C:/VulkanSDK/1.3.280.0/Bin/dxc.exe -T vs_6_0 -E main -spirv -fvk-use-dx-layout -fspv-target-env=vulkan1.0 -D VULKAN=1 -Fo ../Compiled/HLSL/vert.spv vert.hlsl
C:/VulkanSDK/1.3.280.0/Bin/dxc.exe -T cs_6_0 -E main -spirv -fvk-use-dx-layout -fspv-target-env=vulkan1.0 -D VULKAN=1 -Fo ../Compiled/HLSL/particle.spv particle.hlsl 
pause

