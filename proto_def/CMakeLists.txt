
PROTOBUF_GENERATE_CPP(PROTO_SRC PROTO_HEADER base.proto env.proto shader.proto skybox.proto texture.proto mesh.proto)
add_library(proto STATIC ${PROTO_HEADER} ${PROTO_SRC})
