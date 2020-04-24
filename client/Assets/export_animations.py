import bpy
import os
from pathlib import Path
from mathutils import Matrix

export_mesh = True

export_animations = True

def select_type(types):
    first = None
    for o in bpy.context.scene.objects:
        if o.type in types:
            if not first: first = o
            o.select_set(True)
        else:
            o.select_set(False)
    return first
    
output_dir = os.path.splitext(os.path.basename(bpy.context.blend_data.filepath))[0]

Path(output_dir + '/animations').mkdir(parents=True, exist_ok=True)

armature = select_type(['ARMATURE'])

if export_mesh:
    # Reset pose
    for bone in armature.bones:
        bone.matrix_basis = Matrix.Identity(4)

    # Select mesh and armature
    select_type(['ARMATURE', 'MESH'])
        
    # Export
    """
    bpy.ops.export_scene.gltf(
        export_format='GLTF_SEPARATE',
        export_apply=True,
        export_selected=True,
        export_animations=False,
        export_current_frame=True,
        filepath=output_dir + '/riggedMesh',
        check_existing=False
    )
    """
    bpy.ops.export_scene.fbx(
        filepath=output_dir + '/riggedMesh',
        check_existing=False,
        use_selection=True,
        object_types={'ARMATURE','MESH'},
        
    )
    
for a in bpy.data.actions:
    print(a.name)