#ifndef ETERMAL_PRIMITIVE_SHADER_DATA_H_INCLUDED
#define ETERMAL_PRIMITIVE_SHADER_DATA_H_INCLUDED
static const char primitive_vert[] = {
    0x23, 0x76, 0x65, 0x72, 0x73, 0x69, 0x6f, 0x6e, 0x20, 0x33, 0x33, 0x30,
    0x20, 0x63, 0x6f, 0x72, 0x65, 0x0d, 0x0a, 0x6c, 0x61, 0x79, 0x6f, 0x75,
    0x74, 0x20, 0x28, 0x6c, 0x6f, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20,
    0x3d, 0x20, 0x30, 0x29, 0x20, 0x69, 0x6e, 0x20, 0x76, 0x65, 0x63, 0x32,
    0x20, 0x61, 0x50, 0x6f, 0x73, 0x3b, 0x0d, 0x0a, 0x0d, 0x0a, 0x75, 0x6e,
    0x69, 0x66, 0x6f, 0x72, 0x6d, 0x20, 0x6d, 0x61, 0x74, 0x34, 0x20, 0x4d,
    0x6f, 0x64, 0x65, 0x6c, 0x3b, 0x0d, 0x0a, 0x0d, 0x0a, 0x76, 0x6f, 0x69,
    0x64, 0x20, 0x6d, 0x61, 0x69, 0x6e, 0x28, 0x29, 0x0d, 0x0a, 0x7b, 0x0d,
    0x0a, 0x09, 0x67, 0x6c, 0x5f, 0x50, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f,
    0x6e, 0x20, 0x3d, 0x20, 0x4d, 0x6f, 0x64, 0x65, 0x6c, 0x20, 0x2a, 0x20,
    0x76, 0x65, 0x63, 0x34, 0x28, 0x61, 0x50, 0x6f, 0x73, 0x2c, 0x20, 0x30,
    0x2e, 0x30, 0x2c, 0x20, 0x31, 0x2e, 0x30, 0x29, 0x3b, 0x0d, 0x0a, 0x7d,
    0x0d, 0x0a
};
static const int primitive_vert_len = 146;
static const char primitive_frag[] = {
    0x23, 0x76, 0x65, 0x72, 0x73, 0x69, 0x6f, 0x6e, 0x20, 0x33, 0x33, 0x30,
    0x20, 0x63, 0x6f, 0x72, 0x65, 0x0d, 0x0a, 0x6f, 0x75, 0x74, 0x20, 0x76,
    0x65, 0x63, 0x34, 0x20, 0x46, 0x72, 0x61, 0x67, 0x43, 0x6f, 0x6c, 0x6f,
    0x72, 0x3b, 0x0d, 0x0a, 0x0d, 0x0a, 0x75, 0x6e, 0x69, 0x66, 0x6f, 0x72,
    0x6d, 0x20, 0x76, 0x65, 0x63, 0x33, 0x20, 0x43, 0x6f, 0x6c, 0x6f, 0x72,
    0x3b, 0x0d, 0x0a, 0x0d, 0x0a, 0x76, 0x6f, 0x69, 0x64, 0x20, 0x6d, 0x61,
    0x69, 0x6e, 0x28, 0x29, 0x0d, 0x0a, 0x7b, 0x0d, 0x0a, 0x09, 0x46, 0x72,
    0x61, 0x67, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x20, 0x3d, 0x20, 0x76, 0x65,
    0x63, 0x34, 0x28, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x2c, 0x20, 0x31, 0x2e,
    0x30, 0x29, 0x3b, 0x0d, 0x0a, 0x7d, 0x0d, 0x0a
};
static const int primitive_frag_len = 116;
#endif
