#ifndef STATUS_H
#define STATUS_H

/**
 * Status codes for Qengine
 *
 * 0x00 - 0x1F: Success codes
 * 0x20 - 0x5F: Warnings (non-fatal errors)
 * 0x60 - 0x9F: Non-fatal errors (which may be recoverable)
 * 0xA0 - 0xFF: Fatal errors (which typically stop execution)
 */

// Success codes
#define SUCCESS						0x00	// General success
#define SUCCESS_INIT				0x01	// Engine initialized successfully
#define SUCCESS_RENDER				0x02	// Specific rendering task completed successfully
#define SUCCESS_FRAME_RENDERED		0x03	// Entire frame rendered and displayed successfully
#define SUCCESS_RESOURCE_LOADED		0x04	// Resource loaded successfully (e.g. texture or model)
#define 0x05
#define 0x06
#define 0x07
#define 0x08
#define 0x09
#define 0x0A
#define 0x0B
#define 0x0C
#define 0x0D
#define 0x0E
#define 0x0F
#define 0x10
#define 0x11
#define 0x12
#define 0x13
#define 0x14
#define 0x15
#define 0x16
#define 0x17
#define 0x18
#define 0x19
#define 0x1A
#define 0x1B
#define 0x1C
#define 0x1D
#define 0x1E
#define 0x1F

// Warnings (non-fatal errors)
#define WARN_MEMORY_LOW             0x20	// System memory is low, could affect performance
#define 0x21
#define 0x22
#define WARN_DEPRECATED_API         0x23	// Deprecated API function used, may affect compatibility
#define WARN_PARTIAL_FRAME_DROP		0x24	// Frame drop detected; may cause stuttering
#define WARN_TEXTURE_DOWNSCALED		0x25	// High-res texture downscaled due to memory limits
#define WARN_INCOMPLETE_FRAMEBUFFER 0x26	// Framebuffer incomplete, rendering with reduced quality
#define WARN_CPU_LOAD_HIGH          0x27	// CPU load is high; may affect performance
#define WARN_GPU_TEMP_HIGH			0x28	// GPU temperature high; might cause throttling
#define WARN_MISSING_MIPMAPS		0x29	// Texture missing mipmaps, could reduce quality at distance
#define 0x2A
#define 0x2B
#define 0x2C
#define 0x2D
#define 0x2E
#define 0x2F
#define 0x30
#define 0x31
#define 0x32
#define 0x33
#define 0x34
#define 0x35
#define 0x36
#define 0x37
#define 0x38
#define 0x39
#define 0x3A
#define 0x3B
#define 0x3C
#define 0x3D
#define 0x3E
#define 0x3F
#define 0x40
#define 0x41
#define 0x42
#define 0x43
#define 0x44
#define 0x45
#define 0x46
#define 0x47
#define 0x48
#define 0x49
#define 0x4A
#define 0x4B
#define 0x4C
#define 0x4D
#define 0x4E
#define 0x4F
#define 0x50
#define 0x51
#define 0x52
#define 0x53
#define 0x54
#define 0x55
#define 0x56
#define 0x57
#define 0x58
#define 0x59
#define 0x5A
#define 0x5B
#define 0x5C
#define 0x5D
#define 0x5E
#define 0x5F

// Non-fatal errors (which may be recoverable)
#define ERROR_INVALID_PARAMETER				0x60 	// Invalid input parameter detected in function call
#define 0x61
#define 0x62
#define ERROR_TEXTURE_NOT_FOUND				0x63 	// Texture file not found; using placeholder texture
#define ERROR_MODEL_FILE_CORRUPT			0x64 	// Model file is corrupt, unable to load model
#define ERROR_AUDIO_FILE_UNSUPPORTED		0x65 	// Unsupported audio format; cannot load sound resource
#define ERROR_SHADER_SYNTAX					0x66 	// Syntax error in shader code; using fallback shader
#define ERROR_SHADER_UNSUPPORTED_FEATURE	0x67 	// Shader requires unsupported hardware feature
#define ERROR_PIPELINE_CONFIGURATION		0x68 	// Incorrect configuration in rendering pipeline
#define ERROR_UNSUPPORTED_SHADER_VERSION	0x69 	// Shader requires unsupported OpenGL version
#define ERROR_VBO_CREATION_FAILED			0x6A 	// Failed to create Vertex Buffer Object (VBO)
#define ERROR_FBO_INCOMPLETE_ATTACHMENT		0x6B 	// Framebuffer object has incomplete attachment
#define ERROR_ALLOCATION_LIMIT_REACHED		0x6C 	// Memory allocation limit reached, cannot create more resources
#define ERROR_UNSUPPORTED_OS				0x6D 	// Operating system version not fully supported
#define ERROR_UNSUPPORTED_RESOLUTION		0x6E 	// Selected resolution not supported by GPU or display
#define 0x6F
#define 0x70
#define 0x71
#define 0x72
#define 0x73
#define 0x74
#define 0x75
#define 0x76
#define 0x77
#define 0x78
#define 0x79
#define 0x7A
#define 0x7B
#define 0x7C
#define 0x7D
#define 0x7E
#define 0x7F
#define 0x80
#define 0x81
#define 0x82
#define 0x83
#define 0x84
#define 0x85
#define 0x86
#define 0x87
#define 0x88
#define 0x89
#define 0x8A
#define 0x8B
#define 0x8C
#define 0x8D
#define 0x8E
#define 0x8F
#define 0x90
#define 0x91
#define 0x92
#define 0x93
#define 0x94
#define 0x95
#define 0x96
#define 0x97
#define 0x98
#define 0x99
#define 0x9A
#define 0x9B
#define 0x9C
#define 0x9D
#define 0x9E
#define 0x9F

// Fatal errors (which typically stop execution)
#define FATAL_INIT_FAILED					0xA0  	// Engine initialization failed, cannot continue
#define FATAL_CONTEXT_CREATION				0xA1  	// Failed to create OpenGL context
#define FATAL_OUT_OF_MEMORY         		0xA2  	// System out of memory, unable to proceed
#define FATAL_CRASH                 		0xA3  	// Unexpected engine crash or unrecoverable error
#define FATAL_DRIVER_UNSUPPORTED			0xA4  	// Unsupported or incompatible OpenGL driver
#define FATAL_GL_INIT_FAILED        		0xA5  	// OpenGL initialization failed, stopping rendering
#define FATAL_SHADER_COMPILATION_CRASH		0xA6  	// Shader compilation caused system instability
#define FATAL_RESOURCE_OVERLOAD				0xA7  	// Resource load overload causing memory overflow
#define FATAL_TEXTURE_LOAD_FAILURE			0xA8  	// Critical texture failed to load, halting application
#define FATAL_BUFFER_ALLOC_FAILED			0xA9  	// GPU buffer allocation failed, halting rendering
#define FATAL_PIPELINE_ERROR				0xAA  	// Critical rendering pipeline configuration error
#define FATAL_SWAPCHAIN_FAILURE				0xAB  	// Swapchain creation failed, unable to render frames
#define FATAL_SYNC_OBJECT_CREATION_FAILED	0xAC  	// Failed to create sync objects (semaphores/fences)
#define FATAL_FRAMEBUFFER_BINDING			0xAD  	// Framebuffer binding error, halting rendering pipeline
#define FATAL_UNSUPPORTED_GPU				0xAE  	// GPU does not meet minimum hardware requirements
#define FATAL_OUTDATED_DRIVER				0xAF  	// Driver too old for required OpenGL features
#define FATAL_GPU_ACCESS_DENIED				0xB0  	// GPU access denied, possibly due to permissions
#define 0xB1
#define 0xB2
#define 0xB3
#define 0xB4
#define 0xB5
#define 0xB6
#define 0xB7
#define 0xB8
#define 0xB9
#define 0xBA
#define 0xBB
#define 0xBC
#define 0xBD
#define 0xBE
#define 0xBF
#define 0xC0
#define 0xC1
#define 0xC2
#define 0xC3
#define 0xC4
#define 0xC5
#define 0xC6
#define 0xC7
#define 0xC8
#define 0xC9
#define 0xCA
#define 0xCB
#define 0xCC
#define 0xCD
#define 0xCE
#define 0xCF
#define 0xD0
#define 0xD1
#define 0xD2
#define 0xD3
#define 0xD4
#define 0xD5
#define 0xD6
#define 0xD7
#define 0xD8
#define 0xD9
#define 0xDA
#define 0xDB
#define 0xDC
#define 0xDD
#define 0xDE
#define 0xDF
#define 0xE0
#define 0xE1
#define 0xE2
#define 0xE3
#define 0xE4
#define 0xE5
#define 0xE6
#define 0xE7
#define 0xE8
#define 0xE9
#define 0xEA
#define 0xEB
#define 0xEC
#define 0xED
#define 0xEE
#define 0xEF
#define 0xF0
#define 0xF1
#define 0xF2
#define 0xF3
#define 0xF4
#define 0xF5
#define 0xF6
#define 0xF7
#define 0xF8
#define 0xF9
#define 0xFA
#define 0xFB
#define 0xFC
#define 0xFD
#define 0xFE
#define 0xFF

#endif // STATUS_H
