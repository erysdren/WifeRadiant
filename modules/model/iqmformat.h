#ifndef _IQM_H_
#define _IQM_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define IQM_MAGIC ("INTERQUAKEMODEL\0")
#define IQM_MAGIC_SIZE (16)
#define IQM_VERSION (2)

typedef struct iqm_header_s {
	uint8_t magic[IQM_MAGIC_SIZE]; ///< IQM_MAGIC
	uint32_t version; ///< IQM_VERSION
	uint32_t filesize; ///< total size of IQM file, in bytes
	uint32_t flags; ///< unused
	uint32_t num_text, ofs_text;
	uint32_t num_meshes, ofs_meshes;
	uint32_t num_vertex_arrays, num_vertices, ofs_vertex_arrays;
	uint32_t num_triangles, ofs_triangles, ofs_adjacency;
	uint32_t num_joints, ofs_joints;
	uint32_t num_poses, ofs_poses;
	uint32_t num_anims, ofs_anims;
	uint32_t num_frames, num_framechannels, ofs_frames, ofs_bounds;
	uint32_t num_comment, ofs_comment;
	uint32_t num_extensions, ofs_extensions; // these are stored as a linked list, not as a contiguous array
} iqm_header_t;

// helper macros to get IQM components from a file fully loaded into memory
#define IQM_GET_TEXT(iqm) ((char *)(((uint8_t *)(iqm)) + (iqm)->ofs_text))
#define IQM_GET_COMMENT(iqm) ((char *)(((uint8_t *)(iqm)) + (iqm)->ofs_comment))
#define IQM_GET_MESHES(iqm) ((iqm_mesh_t *)(((uint8_t *)(iqm)) + (iqm)->ofs_meshes))
#define IQM_GET_VERTEX_ARRAYS(iqm) ((iqm_vertex_array_t *)(((uint8_t *)(iqm)) + (iqm)->ofs_vertex_arrays))
#define IQM_GET_TRIANGLES(iqm) ((iqm_triangle_t *)(((uint8_t *)(iqm)) + (iqm)->ofs_triangles))
#define IQM_GET_ADJACENCIES(iqm) ((iqm_adjacency_t *)(((uint8_t *)(iqm)) + (iqm)->ofs_adjacency))
#define IQM_GET_JOINTS(iqm) ((iqm_joint_t *)(((uint8_t *)(iqm)) + (iqm)->ofs_joints))
#define IQM_GET_POSES(iqm) ((iqm_pose_t *)(((uint8_t *)(iqm)) + (iqm)->ofs_poses))
#define IQM_GET_ANIMS(iqm) ((iqm_anim_t *)(((uint8_t *)(iqm)) + (iqm)->ofs_anims))
#define IQM_GET_BOUNDS(iqm) ((iqm_bounds_t *)(((uint8_t *)(iqm)) + (iqm)->ofs_bounds))
#define IQM_GET_EXTENSIONS(iqm) ((iqm_extension_t *)(((uint8_t *)(iqm)) + (iqm)->ofs_extensions))

typedef struct iqm_mesh_s {
	uint32_t name;
	uint32_t material;
	uint32_t first_vertex, num_vertices;
	uint32_t first_triangle, num_triangles;
} iqm_mesh_t;

// vertex array type
// all vertex array entries must ordered as defined below, if present
// i.e. position comes before normal comes before ... comes before custom
// where a format and size is given, this means models intended for portable use should use these
// an IQM implementation is not required to honor any other format/size than those recommended
// however, it may support other format/size combinations for these types if it desires
enum {
	IQM_VERTEX_ARRAY_TYPE_POSITION = 0, // float, 3
	IQM_VERTEX_ARRAY_TYPE_TEXCOORD = 1, // float, 2
	IQM_VERTEX_ARRAY_TYPE_NORMAL = 2, // float, 3
	IQM_VERTEX_ARRAY_TYPE_TANGENT = 3, // float, 4
	IQM_VERTEX_ARRAY_TYPE_BLENDINDEXES = 4, // ubyte, 4
	IQM_VERTEX_ARRAY_TYPE_BLENDWEIGHTS = 5, // ubyte, 4
	IQM_VERTEX_ARRAY_TYPE_COLOR = 6, // ubyte, 4

	// all values up to IQM_VERTEX_ARRAY_TYPE_CUSTOM are reserved for future use
	// any value >= IQM_VERTEX_ARRAY_TYPE_CUSTOM is interpreted as CUSTOM type
	// the value then defines an offset into the string table, where offset = value - IQM_VERTEX_ARRAY_TYPE_CUSTOM
	// this must be a valid string naming the type
	IQM_VERTEX_ARRAY_TYPE_CUSTOM = 0x10
};

// vertex array format
enum {
	IQM_VERTEX_ARRAY_FORMAT_S8 = 0, ///< signed byte
	IQM_VERTEX_ARRAY_FORMAT_U8 = 1, ///< unsigned byte
	IQM_VERTEX_ARRAY_FORMAT_S16 = 2, ///< signed 16-bit integer
	IQM_VERTEX_ARRAY_FORMAT_U16 = 3, ///< unsigned 16-bit integer
	IQM_VERTEX_ARRAY_FORMAT_S32 = 4, ///< signed 32-bit integer
	IQM_VERTEX_ARRAY_FORMAT_U32 = 5, ///< unsigned 32-bit integer
	IQM_VERTEX_ARRAY_FORMAT_F16 = 6, ///< 16-bit half precision IEEE floating point
	IQM_VERTEX_ARRAY_FORMAT_F32 = 7, ///< 32-bit single precision IEEE floating point
	IQM_VERTEX_ARRAY_FORMAT_F64 = 8 ///< 64-bit double precision IEEE floating point
};

typedef struct iqm_vertex_array_s {
	uint32_t type; // type or custom name
	uint32_t flags;
	uint32_t format; // component format
	uint32_t size; // number of components
	uint32_t offset; // offset to array of tightly packed components, with num_vertices * size total entries
	// offset must be aligned to max(sizeof(format), 4)
} iqm_vertex_array_t;

typedef struct iqm_triangle_s {
	uint32_t vertices[3];
} iqm_triangle_t;

typedef struct iqm_adjacency_s {
	// each value is the index of the adjacent triangle for edge 0, 1, and 2, where ~0 (= -1) indicates no adjacent triangle
	// indexes are relative to the iqmheader.ofs_triangles array and span all meshes, where 0 is the first triangle, 1 is the second, 2 is the third, etc.
	uint32_t triangle[3];
} iqm_adjacency_t;

typedef struct iqm_joint_s {
	uint32_t name;
	int32_t parent; // parent < 0 means this is a root bone
	float translate[3], rotate[4], scale[3];
	// translate is translation <Tx, Ty, Tz>, and rotate is quaternion rotation <Qx, Qy, Qz, Qw>
	// rotation is in relative/parent local space
	// scale is pre-scaling <Sx, Sy, Sz>
	// output = (input*scale)*rotation + translation
} iqm_joint_t;

typedef struct iqm_pose_s {
	int32_t parent; // parent < 0 means this is a root bone
	uint32_t channelmask; // mask of which 10 channels are present for this joint pose
	float channeloffset[10], channelscale[10];
	// channels 0..2 are translation <Tx, Ty, Tz> and channels 3..6 are quaternion rotation <Qx, Qy, Qz, Qw>
	// rotation is in relative/parent local space
	// channels 7..9 are scale <Sx, Sy, Sz>
	// output = (input*scale)*rotation + translation
} iqm_pose_t;

typedef struct iqm_anim_s {
	uint32_t name;
	uint32_t first_frame, num_frames;
	float framerate;
	uint32_t flags;
} iqm_anim_t;

enum {
	IQM_ANIM_FLAG_LOOP = 1 << 0
};

typedef struct iqm_bounds_s {
	float bbmins[3], bbmaxs[3]; // the minimum and maximum coordinates of the bounding box for this animation frame
	float xyradius, radius; // the circular radius in the X-Y plane, as well as the spherical radius
} iqm_bounds_t;

typedef struct iqm_extension_s {
	uint32_t name;
	uint32_t num_data;
	uint32_t ofs_data;
	uint32_t ofs_next; ///< pointer to next extension
} iqm_extension_t;

#ifdef __cplusplus
}
#endif
#endif // _IQM_H_
