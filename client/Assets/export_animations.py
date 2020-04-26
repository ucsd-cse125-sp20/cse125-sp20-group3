import bpy
import os
from pathlib import Path
from mathutils import Matrix

export_mesh = True
export_animations = True

def select_type(types):
    for o in bpy.context.scene.objects:
        if o.type in types:
            o.select_set(True)
        else:
            o.select_set(False)
    
output_dir = os.path.splitext(bpy.context.blend_data.filepath)[0]
print(os.path.splitext(bpy.context.blend_data.filepath))
try:
    original_umask = os.umask(000)
    os.mkdir(output_dir)
    os.mkdir(output_dir + '/animations')
except FileExistsError:
    print('reusing directory')
finally:
    os.umask(original_umask)

select_type([])

if export_mesh:
    # Reset pose
    for obj in bpy.data.objects:
        if obj.type == 'ARMATURE':
            obj.select_set(True)
            break
    bpy.ops.object.mode_set(mode='EDIT', toggle=False)
    for obj in bpy.data.objects:
        if obj.type == 'ARMATURE':
            obj.pose

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
    
if export_animations:
        
    armature = None
    animated_meshes = []
    for obj in bpy.data.objects:
        if obj.type == 'ARMATURE' and armature is None:
            armature = obj
            obj.select_set(True)
        else:
            obj.select_set(False)
            
    original_action = armature.animation_data.action
    
    for action in bpy.data.actions:
        armature.animation_data_clear()
        anim = armature.animation_data_create()
        anim.action = action
        
        bpy.ops.export_scene.gltf(
            export_format='GLTF_SEPARATE',
            export_apply=True,
            export_selected=True,
            export_animations=True,
            export_force_sampling=True,
            filepath=output_dir + '/animations/' + action.name + '.gltf',
            check_existing=False
        )
        
    armature.animation_data_clear()
    anim = armature.animation_data_create()
    anim.action = original_action