import bpy
import os
from pathlib import Path
from mathutils import Matrix

export_mesh = False

export_animations = True

def select_type(types):
    for o in bpy.context.scene.objects:
        if o.type in types:
            o.select_set(True)
        else:
            o.select_set(False)
    
output_dir = os.path.splitext(os.path.basename(bpy.context.blend_data.filepath))[0]

Path(output_dir + '/animations').mkdir(parents=True, exist_ok=True)

if export_mesh:
    # Reset pose
    for pb in bpy.context.selected_pose_bones_from_active_object:
        pb.matrix_basis = Matrix.Identity(4)

    # Select mesh and armature
    select_type(['ARMATURE', 'MESH'])
        
    # Export
    bpy.ops.export_scene.gltf(
        export_format='GLTF_SEPARATE',
        export_apply=True,
        export_selected=True,
        export_animations=False,
        export_current_frame=True,
        filepath=output_dir + '/riggedMesh.gltf',
        check_existing=False
    )
    
