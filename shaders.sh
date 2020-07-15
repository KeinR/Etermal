#!/usr/bin/env bash

# Create shader embedables

pushd "resources/shaders" >/dev/null

function comp {
    cat <<EOF > ${1}.h
#ifndef ETERMAL_PRIMITIVE_SHADER_DATA_H_INCLUDED
#define ETERMAL_PRIMITIVE_SHADER_DATA_H_INCLUDED
`xxd -i ${1}.vert`
`xxd -i ${1}.frag`
#endif
EOF
    sed -E -i\
        -e 's/unsigned char/static const char/g'\
        -e 's/unsigned int/static int/g'\
        -e 's/  /    /g'\
        ${1}.h
}

comp "primitive"
comp "text"

popd >/dev/null