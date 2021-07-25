import glob
import re
import os
import sys


webgl_build = True if len(sys.argv) > 1 and 'webgl' in sys.argv[1] else False

with open('shaders.hpp', 'w') as target_file:
    target_file.write('#ifndef _SHADERS_H_\n#define _SHADERS_H_\n')
    # target_file.write('#include "gl_wrappers2d/gl_wrappers.h"\n\n')
    for filename in glob.glob("./shaders/*", recursive=True):
        if sys.platform == 'win32':
            filename2 = filename.split('\\')[-1]
        else:
            filename2 = filename.split('/')[-1]
        tok = filename2.split('.')
        s_type = tok.pop()
        shader_src_name = '_'.join(''.join(tok).split('-'))
        shader_name = shader_src_name + '_shader'
        shader_src_name += '_shader_source'
        with open(filename, 'r') as f2:
            shader_contents = f2.read()
            if webgl_build:
                shader_contents = re.sub('#[ ]*version[ ]+330[ ]+core',
                                         '', shader_contents)
            target_file.write(f'const char *{shader_src_name} '
                               + f'= R"({shader_contents})";\n')
    target_file.write('#endif')
