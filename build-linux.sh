mkdir tmp

echo Compiling freeglut3-linux...
mkdir tmp/freeglut3-linux
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/fg_callbacks.c -o tmp/freeglut3-linux/fg_callbacks.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/fg_cursor.c -o tmp/freeglut3-linux/fg_cursor.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/fg_display.c -o tmp/freeglut3-linux/fg_display.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/fg_ext.c -o tmp/freeglut3-linux/fg_ext.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/fg_font.c -o tmp/freeglut3-linux/fg_font.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/fg_font_data.c -o tmp/freeglut3-linux/fg_font_data.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/fg_gamemode.c -o tmp/freeglut3-linux/fg_gamemode.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/fg_geometry.c -o tmp/freeglut3-linux/fg_geometry.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/fg_gl2.c -o tmp/freeglut3-linux/fg_gl2.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/fg_init.c -o tmp/freeglut3-linux/fg_init.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/fg_input_devices.c -o tmp/freeglut3-linux/fg_input_devices.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/fg_joystick.c -o tmp/freeglut3-linux/fg_joystick.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/fg_main.c -o tmp/freeglut3-linux/fg_main.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/fg_menu.c -o tmp/freeglut3-linux/fg_menu.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/fg_misc.c -o tmp/freeglut3-linux/fg_misc.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/fg_overlay.c -o tmp/freeglut3-linux/fg_overlay.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/fg_spaceball.c -o tmp/freeglut3-linux/fg_spaceball.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/fg_state.c -o tmp/freeglut3-linux/fg_state.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/fg_stroke_mono_roman.c -o tmp/freeglut3-linux/fg_stroke_mono_roman.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/fg_stroke_roman.c -o tmp/freeglut3-linux/fg_stroke_roman.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/fg_structure.c -o tmp/freeglut3-linux/fg_structure.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/fg_teapot.c -o tmp/freeglut3-linux/fg_teapot.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/fg_videoresize.c -o tmp/freeglut3-linux/fg_videoresize.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/fg_window.c -o tmp/freeglut3-linux/fg_window.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/util/xparsegeometry_repl.c -o tmp/freeglut3-linux/xparsegeometry_repl.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/x11/fg_cursor_x11.c -o tmp/freeglut3-linux/fg_cursor_x11.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/x11/fg_display_x11_glx.c -o tmp/freeglut3-linux/fg_display_x11_glx.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/x11/fg_ext_x11.c -o tmp/freeglut3-linux/fg_ext_x11.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/x11/fg_gamemode_x11.c -o tmp/freeglut3-linux/fg_gamemode_x11.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/x11/fg_glutfont_definitions_x11.c -o tmp/freeglut3-linux/fg_glutfont_definitions_x11.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/x11/fg_init_x11.c -o tmp/freeglut3-linux/fg_init_x11.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/x11/fg_input_devices_x11.c -o tmp/freeglut3-linux/fg_input_devices_x11.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/x11/fg_joystick_x11.c -o tmp/freeglut3-linux/fg_joystick_x11.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/x11/fg_main_x11.c -o tmp/freeglut3-linux/fg_main_x11.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/x11/fg_menu_x11.c -o tmp/freeglut3-linux/fg_menu_x11.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/x11/fg_spaceball_x11.c -o tmp/freeglut3-linux/fg_spaceball_x11.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/x11/fg_state_x11.c -o tmp/freeglut3-linux/fg_state_x11.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/x11/fg_state_x11_glx.c -o tmp/freeglut3-linux/fg_state_x11_glx.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/x11/fg_structure_x11.c -o tmp/freeglut3-linux/fg_structure_x11.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/x11/fg_window_x11.c -o tmp/freeglut3-linux/fg_window_x11.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/x11/fg_window_x11_glx.c -o tmp/freeglut3-linux/fg_window_x11_glx.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/freeglut3/src/x11/fg_xinput_x11.c -o tmp/freeglut3-linux/fg_xinput_x11.o
ar rcs tmp/freeglut3-linux/freeglut3-linux.a tmp/freeglut3-linux/*.o 
echo ...Finished

echo Compiling glew2-linux...
mkdir tmp/glew2-linux
g++ -c -fPIC -w -x c -std=c11 3rd-party/glew2/src/glew.c -o tmp/glew2-linux/glew.o
ar rcs tmp/glew2-linux/glew2-linux.a tmp/glew2-linux/*.o 
echo ...Finished

echo Compiling GeometryLib-linux...
mkdir tmp/GeometryLib-linux
g++ -c -fPIC -w -x c++ -std=c++11 tools/GeometryLib/bounding-box.cpp -o tmp/GeometryLib-linux/bounding-box.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/GeometryLib/bounding-cylinder.cpp -o tmp/GeometryLib-linux/bounding-cylinder.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/GeometryLib/frustum.cpp -o tmp/GeometryLib-linux/frustum.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/GeometryLib/matrix44.cpp -o tmp/GeometryLib-linux/matrix44.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/GeometryLib/plane.cpp -o tmp/GeometryLib-linux/plane.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/GeometryLib/quaternion.cpp -o tmp/GeometryLib-linux/quaternion.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/GeometryLib/transform2d.cpp -o tmp/GeometryLib-linux/transform2d.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/GeometryLib/transform3d.cpp -o tmp/GeometryLib-linux/transform3d.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/GeometryLib/vector2d.cpp -o tmp/GeometryLib-linux/vector2d.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/GeometryLib/vector3d.cpp -o tmp/GeometryLib-linux/vector3d.o
ar rcs tmp/GeometryLib-linux/GeometryLib-linux.a tmp/GeometryLib-linux/*.o 
echo ...Finished

echo Compiling SOIL-linux...
mkdir tmp/SOIL-linux
g++ -c -fPIC -w -x c -std=c11 3rd-party/SOIL/src/image_DXT.c -o tmp/SOIL-linux/image_DXT.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/SOIL/src/image_helper.c -o tmp/SOIL-linux/image_helper.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/SOIL/src/SOIL.c -o tmp/SOIL-linux/SOIL.o
g++ -c -fPIC -w -x c -std=c11 3rd-party/SOIL/src/stb_image_aug.c -o tmp/SOIL-linux/stb_image_aug.o
ar rcs tmp/SOIL-linux/SOIL-linux.a tmp/SOIL-linux/*.o 
echo ...Finished

echo Compiling System-linux...
mkdir tmp/System-linux
g++ -c -fPIC -w -x c++ -std=c++11 tools/System/CrossPlatform.cpp -o tmp/System-linux/CrossPlatform.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/System/DynamicLib-linux.cpp -o tmp/System-linux/DynamicLib-linux.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/System/FileUtils.cpp -o tmp/System-linux/FileUtils.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/System/NamedPipe-Common.cpp -o tmp/System-linux/NamedPipe-Common.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/System/NamedPipe-linux.cpp -o tmp/System-linux/NamedPipe-linux.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/System/Process-linux.cpp -o tmp/System-linux/Process-linux.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/System/Timer.cpp -o tmp/System-linux/Timer.o
ar rcs tmp/System-linux/System-linux.a tmp/System-linux/*.o 
echo ...Finished

echo Compiling tinyxml2-linux...
mkdir tmp/tinyxml2-linux
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/tinyxml2/tinyxml2.cpp -o tmp/tinyxml2-linux/tinyxml2.o
ar rcs tmp/tinyxml2-linux/tinyxml2-linux.a tmp/tinyxml2-linux/*.o 
echo ...Finished

echo Compiling OpenGLRenderer-linux...
mkdir tmp/OpenGLRenderer-linux
g++ -c -fPIC -w -x c++ -std=c++11 tools/OpenGLRenderer/arranger.cpp -o tmp/OpenGLRenderer-linux/arranger.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/OpenGLRenderer/basic-shapes-2d.cpp -o tmp/OpenGLRenderer-linux/basic-shapes-2d.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/OpenGLRenderer/basic-shapes-3d.cpp -o tmp/OpenGLRenderer-linux/basic-shapes-3d.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/OpenGLRenderer/light.cpp -o tmp/OpenGLRenderer-linux/light.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/OpenGLRenderer/material-live-texture.cpp -o tmp/OpenGLRenderer-linux/material-live-texture.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/OpenGLRenderer/mesh.cpp -o tmp/OpenGLRenderer-linux/mesh.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/OpenGLRenderer/bindings.cpp -o tmp/OpenGLRenderer-linux/bindings.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/OpenGLRenderer/graphic-object-2d.cpp -o tmp/OpenGLRenderer-linux/graphic-object-2d.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/OpenGLRenderer/camera.cpp -o tmp/OpenGLRenderer-linux/camera.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/OpenGLRenderer/collada-model.cpp -o tmp/OpenGLRenderer-linux/collada-model.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/OpenGLRenderer/ogl-utils.cpp -o tmp/OpenGLRenderer-linux/ogl-utils.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/OpenGLRenderer/scene-actor-2d.cpp -o tmp/OpenGLRenderer-linux/scene-actor-2d.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/OpenGLRenderer/scene-actor-3d.cpp -o tmp/OpenGLRenderer-linux/scene-actor-3d.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/OpenGLRenderer/graphic-object-3d.cpp -o tmp/OpenGLRenderer-linux/graphic-object-3d.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/OpenGLRenderer/input-handler.cpp -o tmp/OpenGLRenderer-linux/input-handler.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/OpenGLRenderer/material.cpp -o tmp/OpenGLRenderer-linux/material.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/OpenGLRenderer/renderer.cpp -o tmp/OpenGLRenderer-linux/renderer.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/OpenGLRenderer/color.cpp -o tmp/OpenGLRenderer-linux/color.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/OpenGLRenderer/text.cpp -o tmp/OpenGLRenderer-linux/text.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/OpenGLRenderer/texture-manager.cpp -o tmp/OpenGLRenderer-linux/texture-manager.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/OpenGLRenderer/viewport.cpp -o tmp/OpenGLRenderer-linux/viewport.o
g++ -c -fPIC -w -x c++ -std=c++11 tools/OpenGLRenderer/xml-load.cpp -o tmp/OpenGLRenderer-linux/xml-load.o
ar rcs tmp/OpenGLRenderer-linux/OpenGLRenderer-linux.a tmp/OpenGLRenderer-linux/*.o 
echo ...Finished

echo Compiling Bullet3-linux...
mkdir tmp/Bullet3-linux
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/Bullet3Common/b3AlignedAllocator.cpp -o tmp/Bullet3-linux/b3AlignedAllocator.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/Bullet3Common/b3Logging.cpp -o tmp/Bullet3-linux/b3Logging.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/Bullet3Common/b3Vector3.cpp -o tmp/Bullet3-linux/b3Vector3.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/Bullet3Geometry/b3ConvexHullComputer.cpp -o tmp/Bullet3-linux/b3ConvexHullComputer.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/Bullet3Geometry/b3GeometryUtil.cpp -o tmp/Bullet3-linux/b3GeometryUtil.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/BroadphaseCollision/btAxisSweep3.cpp -o tmp/Bullet3-linux/btAxisSweep3.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/BroadphaseCollision/btCollisionAlgorithm.cpp -o tmp/Bullet3-linux/btCollisionAlgorithm.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/BroadphaseCollision/btDbvt.cpp -o tmp/Bullet3-linux/btDbvt.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/BroadphaseCollision/btDbvtBroadphase.cpp -o tmp/Bullet3-linux/btDbvtBroadphase.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/BroadphaseCollision/btDispatcher.cpp -o tmp/Bullet3-linux/btDispatcher.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/BroadphaseCollision/btOverlappingPairCache.cpp -o tmp/Bullet3-linux/btOverlappingPairCache.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/BroadphaseCollision/btQuantizedBvh.cpp -o tmp/Bullet3-linux/btQuantizedBvh.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/BroadphaseCollision/btSimpleBroadphase.cpp -o tmp/Bullet3-linux/btSimpleBroadphase.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionDispatch/btActivatingCollisionAlgorithm.cpp -o tmp/Bullet3-linux/btActivatingCollisionAlgorithm.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionDispatch/btBox2dBox2dCollisionAlgorithm.cpp -o tmp/Bullet3-linux/btBox2dBox2dCollisionAlgorithm.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionDispatch/btBoxBoxCollisionAlgorithm.cpp -o tmp/Bullet3-linux/btBoxBoxCollisionAlgorithm.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionDispatch/btBoxBoxDetector.cpp -o tmp/Bullet3-linux/btBoxBoxDetector.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionDispatch/btCollisionDispatcher.cpp -o tmp/Bullet3-linux/btCollisionDispatcher.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionDispatch/btCollisionObject.cpp -o tmp/Bullet3-linux/btCollisionObject.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionDispatch/btCollisionWorld.cpp -o tmp/Bullet3-linux/btCollisionWorld.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionDispatch/btCollisionWorldImporter.cpp -o tmp/Bullet3-linux/btCollisionWorldImporter.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionDispatch/btCompoundCollisionAlgorithm.cpp -o tmp/Bullet3-linux/btCompoundCollisionAlgorithm.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionDispatch/btCompoundCompoundCollisionAlgorithm.cpp -o tmp/Bullet3-linux/btCompoundCompoundCollisionAlgorithm.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionDispatch/btConvex2dConvex2dAlgorithm.cpp -o tmp/Bullet3-linux/btConvex2dConvex2dAlgorithm.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionDispatch/btConvexConcaveCollisionAlgorithm.cpp -o tmp/Bullet3-linux/btConvexConcaveCollisionAlgorithm.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionDispatch/btConvexConvexAlgorithm.cpp -o tmp/Bullet3-linux/btConvexConvexAlgorithm.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionDispatch/btConvexPlaneCollisionAlgorithm.cpp -o tmp/Bullet3-linux/btConvexPlaneCollisionAlgorithm.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.cpp -o tmp/Bullet3-linux/btDefaultCollisionConfiguration.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionDispatch/btEmptyCollisionAlgorithm.cpp -o tmp/Bullet3-linux/btEmptyCollisionAlgorithm.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionDispatch/btGhostObject.cpp -o tmp/Bullet3-linux/btGhostObject.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionDispatch/btHashedSimplePairCache.cpp -o tmp/Bullet3-linux/btHashedSimplePairCache.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionDispatch/btInternalEdgeUtility.cpp -o tmp/Bullet3-linux/btInternalEdgeUtility.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionDispatch/btManifoldResult.cpp -o tmp/Bullet3-linux/btManifoldResult.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionDispatch/btSimulationIslandManager.cpp -o tmp/Bullet3-linux/btSimulationIslandManager.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionDispatch/btSphereBoxCollisionAlgorithm.cpp -o tmp/Bullet3-linux/btSphereBoxCollisionAlgorithm.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionDispatch/btSphereSphereCollisionAlgorithm.cpp -o tmp/Bullet3-linux/btSphereSphereCollisionAlgorithm.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionDispatch/btSphereTriangleCollisionAlgorithm.cpp -o tmp/Bullet3-linux/btSphereTriangleCollisionAlgorithm.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionDispatch/btUnionFind.cpp -o tmp/Bullet3-linux/btUnionFind.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionDispatch/SphereTriangleDetector.cpp -o tmp/Bullet3-linux/SphereTriangleDetector.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btBox2dShape.cpp -o tmp/Bullet3-linux/btBox2dShape.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btBoxShape.cpp -o tmp/Bullet3-linux/btBoxShape.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btBvhTriangleMeshShape.cpp -o tmp/Bullet3-linux/btBvhTriangleMeshShape.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btCapsuleShape.cpp -o tmp/Bullet3-linux/btCapsuleShape.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btCollisionShape.cpp -o tmp/Bullet3-linux/btCollisionShape.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btCompoundShape.cpp -o tmp/Bullet3-linux/btCompoundShape.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btConcaveShape.cpp -o tmp/Bullet3-linux/btConcaveShape.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btConeShape.cpp -o tmp/Bullet3-linux/btConeShape.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btConvex2dShape.cpp -o tmp/Bullet3-linux/btConvex2dShape.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btConvexHullShape.cpp -o tmp/Bullet3-linux/btConvexHullShape.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btConvexInternalShape.cpp -o tmp/Bullet3-linux/btConvexInternalShape.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btConvexPointCloudShape.cpp -o tmp/Bullet3-linux/btConvexPointCloudShape.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btConvexPolyhedron.cpp -o tmp/Bullet3-linux/btConvexPolyhedron.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btConvexShape.cpp -o tmp/Bullet3-linux/btConvexShape.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btConvexTriangleMeshShape.cpp -o tmp/Bullet3-linux/btConvexTriangleMeshShape.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btCylinderShape.cpp -o tmp/Bullet3-linux/btCylinderShape.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btEmptyShape.cpp -o tmp/Bullet3-linux/btEmptyShape.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.cpp -o tmp/Bullet3-linux/btHeightfieldTerrainShape.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btMinkowskiSumShape.cpp -o tmp/Bullet3-linux/btMinkowskiSumShape.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btMultimaterialTriangleMeshShape.cpp -o tmp/Bullet3-linux/btMultimaterialTriangleMeshShape.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btMultiSphereShape.cpp -o tmp/Bullet3-linux/btMultiSphereShape.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btOptimizedBvh.cpp -o tmp/Bullet3-linux/btOptimizedBvh.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btPolyhedralConvexShape.cpp -o tmp/Bullet3-linux/btPolyhedralConvexShape.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btScaledBvhTriangleMeshShape.cpp -o tmp/Bullet3-linux/btScaledBvhTriangleMeshShape.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btShapeHull.cpp -o tmp/Bullet3-linux/btShapeHull.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btSphereShape.cpp -o tmp/Bullet3-linux/btSphereShape.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btStaticPlaneShape.cpp -o tmp/Bullet3-linux/btStaticPlaneShape.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btStridingMeshInterface.cpp -o tmp/Bullet3-linux/btStridingMeshInterface.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btTetrahedronShape.cpp -o tmp/Bullet3-linux/btTetrahedronShape.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btTriangleBuffer.cpp -o tmp/Bullet3-linux/btTriangleBuffer.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btTriangleCallback.cpp -o tmp/Bullet3-linux/btTriangleCallback.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btTriangleIndexVertexArray.cpp -o tmp/Bullet3-linux/btTriangleIndexVertexArray.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btTriangleIndexVertexMaterialArray.cpp -o tmp/Bullet3-linux/btTriangleIndexVertexMaterialArray.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btTriangleMesh.cpp -o tmp/Bullet3-linux/btTriangleMesh.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btTriangleMeshShape.cpp -o tmp/Bullet3-linux/btTriangleMeshShape.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/CollisionShapes/btUniformScalingShape.cpp -o tmp/Bullet3-linux/btUniformScalingShape.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/Gimpact/btContactProcessing.cpp -o tmp/Bullet3-linux/btContactProcessing.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/Gimpact/btGenericPoolAllocator.cpp -o tmp/Bullet3-linux/btGenericPoolAllocator.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/Gimpact/btGImpactBvh.cpp -o tmp/Bullet3-linux/btGImpactBvh.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/Gimpact/btGImpactCollisionAlgorithm.cpp -o tmp/Bullet3-linux/btGImpactCollisionAlgorithm.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/Gimpact/btGImpactQuantizedBvh.cpp -o tmp/Bullet3-linux/btGImpactQuantizedBvh.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/Gimpact/btGImpactShape.cpp -o tmp/Bullet3-linux/btGImpactShape.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/Gimpact/btTriangleShapeEx.cpp -o tmp/Bullet3-linux/btTriangleShapeEx.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/Gimpact/gim_box_set.cpp -o tmp/Bullet3-linux/gim_box_set.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/Gimpact/gim_contact.cpp -o tmp/Bullet3-linux/gim_contact.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/Gimpact/gim_memory.cpp -o tmp/Bullet3-linux/gim_memory.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/Gimpact/gim_tri_collision.cpp -o tmp/Bullet3-linux/gim_tri_collision.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/NarrowPhaseCollision/btContinuousConvexCollision.cpp -o tmp/Bullet3-linux/btContinuousConvexCollision.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/NarrowPhaseCollision/btConvexCast.cpp -o tmp/Bullet3-linux/btConvexCast.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/NarrowPhaseCollision/btGjkConvexCast.cpp -o tmp/Bullet3-linux/btGjkConvexCast.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/NarrowPhaseCollision/btGjkEpa2.cpp -o tmp/Bullet3-linux/btGjkEpa2.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/NarrowPhaseCollision/btGjkEpaPenetrationDepthSolver.cpp -o tmp/Bullet3-linux/btGjkEpaPenetrationDepthSolver.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/NarrowPhaseCollision/btGjkPairDetector.cpp -o tmp/Bullet3-linux/btGjkPairDetector.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/NarrowPhaseCollision/btMinkowskiPenetrationDepthSolver.cpp -o tmp/Bullet3-linux/btMinkowskiPenetrationDepthSolver.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/NarrowPhaseCollision/btPersistentManifold.cpp -o tmp/Bullet3-linux/btPersistentManifold.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/NarrowPhaseCollision/btPolyhedralContactClipping.cpp -o tmp/Bullet3-linux/btPolyhedralContactClipping.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/NarrowPhaseCollision/btRaycastCallback.cpp -o tmp/Bullet3-linux/btRaycastCallback.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/NarrowPhaseCollision/btSubSimplexConvexCast.cpp -o tmp/Bullet3-linux/btSubSimplexConvexCast.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletCollision/NarrowPhaseCollision/btVoronoiSimplexSolver.cpp -o tmp/Bullet3-linux/btVoronoiSimplexSolver.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/Bullet3Collision/BroadPhaseCollision/b3DynamicBvh.cpp -o tmp/Bullet3-linux/b3DynamicBvh.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/Bullet3Collision/BroadPhaseCollision/b3DynamicBvhBroadphase.cpp -o tmp/Bullet3-linux/b3DynamicBvhBroadphase.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/Bullet3Collision/BroadPhaseCollision/b3OverlappingPairCache.cpp -o tmp/Bullet3-linux/b3OverlappingPairCache.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/Bullet3Collision/NarrowPhaseCollision/b3ConvexUtility.cpp -o tmp/Bullet3-linux/b3ConvexUtility.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/Bullet3Collision/NarrowPhaseCollision/b3CpuNarrowPhase.cpp -o tmp/Bullet3-linux/b3CpuNarrowPhase.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/Bullet3Dynamics/b3CpuRigidBodyPipeline.cpp -o tmp/Bullet3-linux/b3CpuRigidBodyPipeline.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/Bullet3Dynamics/ConstraintSolver/b3FixedConstraint.cpp -o tmp/Bullet3-linux/b3FixedConstraint.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/Bullet3Dynamics/ConstraintSolver/b3Generic6DofConstraint.cpp -o tmp/Bullet3-linux/b3Generic6DofConstraint.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/Bullet3Dynamics/ConstraintSolver/b3PgsJacobiSolver.cpp -o tmp/Bullet3-linux/b3PgsJacobiSolver.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/Bullet3Dynamics/ConstraintSolver/b3Point2PointConstraint.cpp -o tmp/Bullet3-linux/b3Point2PointConstraint.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/Bullet3Dynamics/ConstraintSolver/b3TypedConstraint.cpp -o tmp/Bullet3-linux/b3TypedConstraint.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletSoftBody/btDefaultSoftBodySolver.cpp -o tmp/Bullet3-linux/btDefaultSoftBodySolver.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletSoftBody/btSoftBody.cpp -o tmp/Bullet3-linux/btSoftBody.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletSoftBody/btSoftBodyConcaveCollisionAlgorithm.cpp -o tmp/Bullet3-linux/btSoftBodyConcaveCollisionAlgorithm.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletSoftBody/btSoftBodyHelpers.cpp -o tmp/Bullet3-linux/btSoftBodyHelpers.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.cpp -o tmp/Bullet3-linux/btSoftBodyRigidBodyCollisionConfiguration.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletSoftBody/btSoftMultiBodyDynamicsWorld.cpp -o tmp/Bullet3-linux/btSoftMultiBodyDynamicsWorld.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletSoftBody/btSoftRigidCollisionAlgorithm.cpp -o tmp/Bullet3-linux/btSoftRigidCollisionAlgorithm.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletSoftBody/btSoftRigidDynamicsWorld.cpp -o tmp/Bullet3-linux/btSoftRigidDynamicsWorld.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletSoftBody/btSoftSoftCollisionAlgorithm.cpp -o tmp/Bullet3-linux/btSoftSoftCollisionAlgorithm.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/LinearMath/btAlignedAllocator.cpp -o tmp/Bullet3-linux/btAlignedAllocator.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/LinearMath/btConvexHull.cpp -o tmp/Bullet3-linux/btConvexHull.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/LinearMath/btConvexHullComputer.cpp -o tmp/Bullet3-linux/btConvexHullComputer.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/LinearMath/btGeometryUtil.cpp -o tmp/Bullet3-linux/btGeometryUtil.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/LinearMath/btPolarDecomposition.cpp -o tmp/Bullet3-linux/btPolarDecomposition.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/LinearMath/btQuickprof.cpp -o tmp/Bullet3-linux/btQuickprof.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/LinearMath/btSerializer.cpp -o tmp/Bullet3-linux/btSerializer.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/LinearMath/btThreads.cpp -o tmp/Bullet3-linux/btThreads.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/LinearMath/btVector3.cpp -o tmp/Bullet3-linux/btVector3.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.cpp -o tmp/Bullet3-linux/btDiscreteDynamicsWorld.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/Dynamics/btDiscreteDynamicsWorldMt.cpp -o tmp/Bullet3-linux/btDiscreteDynamicsWorldMt.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/Dynamics/btRigidBody.cpp -o tmp/Bullet3-linux/btRigidBody.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/Dynamics/btSimpleDynamicsWorld.cpp -o tmp/Bullet3-linux/btSimpleDynamicsWorld.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/Dynamics/btSimulationIslandManagerMt.cpp -o tmp/Bullet3-linux/btSimulationIslandManagerMt.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/ConstraintSolver/btConeTwistConstraint.cpp -o tmp/Bullet3-linux/btConeTwistConstraint.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/ConstraintSolver/btContactConstraint.cpp -o tmp/Bullet3-linux/btContactConstraint.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/ConstraintSolver/btFixedConstraint.cpp -o tmp/Bullet3-linux/btFixedConstraint.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/ConstraintSolver/btGearConstraint.cpp -o tmp/Bullet3-linux/btGearConstraint.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.cpp -o tmp/Bullet3-linux/btGeneric6DofConstraint.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/ConstraintSolver/btGeneric6DofSpring2Constraint.cpp -o tmp/Bullet3-linux/btGeneric6DofSpring2Constraint.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/ConstraintSolver/btGeneric6DofSpringConstraint.cpp -o tmp/Bullet3-linux/btGeneric6DofSpringConstraint.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/ConstraintSolver/btHinge2Constraint.cpp -o tmp/Bullet3-linux/btHinge2Constraint.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/ConstraintSolver/btHingeConstraint.cpp -o tmp/Bullet3-linux/btHingeConstraint.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/ConstraintSolver/btNNCGConstraintSolver.cpp -o tmp/Bullet3-linux/btNNCGConstraintSolver.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/ConstraintSolver/btPoint2PointConstraint.cpp -o tmp/Bullet3-linux/btPoint2PointConstraint.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.cpp -o tmp/Bullet3-linux/btSequentialImpulseConstraintSolver.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/ConstraintSolver/btSliderConstraint.cpp -o tmp/Bullet3-linux/btSliderConstraint.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/ConstraintSolver/btSolve2LinearConstraint.cpp -o tmp/Bullet3-linux/btSolve2LinearConstraint.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/ConstraintSolver/btTypedConstraint.cpp -o tmp/Bullet3-linux/btTypedConstraint.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/ConstraintSolver/btUniversalConstraint.cpp -o tmp/Bullet3-linux/btUniversalConstraint.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/Featherstone/btMultiBody.cpp -o tmp/Bullet3-linux/btMultiBody.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/Featherstone/btMultiBodyConstraint.cpp -o tmp/Bullet3-linux/btMultiBodyConstraint.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/Featherstone/btMultiBodyConstraintSolver.cpp -o tmp/Bullet3-linux/btMultiBodyConstraintSolver.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/Featherstone/btMultiBodyDynamicsWorld.cpp -o tmp/Bullet3-linux/btMultiBodyDynamicsWorld.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/Featherstone/btMultiBodyFixedConstraint.cpp -o tmp/Bullet3-linux/btMultiBodyFixedConstraint.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/Featherstone/btMultiBodyJointLimitConstraint.cpp -o tmp/Bullet3-linux/btMultiBodyJointLimitConstraint.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/Featherstone/btMultiBodyJointMotor.cpp -o tmp/Bullet3-linux/btMultiBodyJointMotor.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/Featherstone/btMultiBodyPoint2Point.cpp -o tmp/Bullet3-linux/btMultiBodyPoint2Point.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/Featherstone/btMultiBodySliderConstraint.cpp -o tmp/Bullet3-linux/btMultiBodySliderConstraint.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/MLCPSolvers/btDantzigLCP.cpp -o tmp/Bullet3-linux/btDantzigLCP.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/MLCPSolvers/btLemkeAlgorithm.cpp -o tmp/Bullet3-linux/btLemkeAlgorithm.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/MLCPSolvers/btMLCPSolver.cpp -o tmp/Bullet3-linux/btMLCPSolver.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/Vehicle/btRaycastVehicle.cpp -o tmp/Bullet3-linux/btRaycastVehicle.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/Vehicle/btWheelInfo.cpp -o tmp/Bullet3-linux/btWheelInfo.o
g++ -c -fPIC -w -x c++ -std=c++11 3rd-party/bullet3-2.86/src/BulletDynamics/Character/btKinematicCharacterController.cpp -o tmp/Bullet3-linux/btKinematicCharacterController.o
ar rcs tmp/Bullet3-linux/Bullet3-linux.a tmp/Bullet3-linux/*.o 
echo ...Finished

echo Compiling RLSimion-Common-linux...
mkdir tmp/RLSimion-Common-linux
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Common/named-var-set.cpp -o tmp/RLSimion-Common-linux/named-var-set.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Common/wire.cpp -o tmp/RLSimion-Common-linux/wire.o
ar rcs tmp/RLSimion-Common-linux/RLSimion-Common-linux.a tmp/RLSimion-Common-linux/*.o 
echo ...Finished

echo Compiling CNTKWrapper-linux...
mkdir tmp/CNTKWrapper-linux
cp RLSimion/CNTKWrapper/../../bin/cntk-linux/libCntk.Core-2.5.1.so tmp/CNTKWrapper-linux/
cp RLSimion/CNTKWrapper/../../bin/cntk-linux/libCntk.Math-2.5.1.so tmp/CNTKWrapper-linux/
cp RLSimion/CNTKWrapper/../../bin/cntk-linux/libCntk.PerformanceProfiler-2.5.1.so tmp/CNTKWrapper-linux/
cp RLSimion/CNTKWrapper/../../bin/cntk-linux/libiomp5.so tmp/CNTKWrapper-linux/
cp RLSimion/CNTKWrapper/../../bin/cntk-linux/libmklml_intel.so tmp/CNTKWrapper-linux/
cp RLSimion/CNTKWrapper/../../bin/cntk-linux/libmpi.so.12 tmp/CNTKWrapper-linux/
cp RLSimion/CNTKWrapper/../../bin/cntk-linux/libmpi_cxx.so.1 tmp/CNTKWrapper-linux/
cp RLSimion/CNTKWrapper/../../bin/cntk-linux/libopen-pal.so.13 tmp/CNTKWrapper-linux/
cp RLSimion/CNTKWrapper/../../bin/cntk-linux/libopen-rte.so.12 tmp/CNTKWrapper-linux/
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/CNTKWrapper/Chain.cpp -o tmp/CNTKWrapper-linux/Chain.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/CNTKWrapper/CNTKWrapper.cpp -o tmp/CNTKWrapper-linux/CNTKWrapper.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/CNTKWrapper/CNTKWrapperInternals.cpp -o tmp/CNTKWrapper-linux/CNTKWrapperInternals.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/CNTKWrapper/Minibatch.cpp -o tmp/CNTKWrapper-linux/Minibatch.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/CNTKWrapper/OptimizerSetting.cpp -o tmp/CNTKWrapper-linux/OptimizerSetting.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/CNTKWrapper/NetworkDefinition.cpp -o tmp/CNTKWrapper-linux/NetworkDefinition.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/CNTKWrapper/InputData.cpp -o tmp/CNTKWrapper-linux/InputData.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/CNTKWrapper/Link.cpp -o tmp/CNTKWrapper-linux/Link.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/CNTKWrapper/Network.cpp -o tmp/CNTKWrapper-linux/Network.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/CNTKWrapper/NetworkArchitecture.cpp -o tmp/CNTKWrapper-linux/NetworkArchitecture.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/CNTKWrapper/Parameter.cpp -o tmp/CNTKWrapper-linux/Parameter.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/CNTKWrapper/ParameterValues.cpp -o tmp/CNTKWrapper-linux/ParameterValues.o
g++ -o tmp/CNTKWrapper-linux/CNTKWrapper-linux.so tmp/CNTKWrapper-linux/*.o -Wl,--no-undefined -Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack -shared  "tmp/tinyxml2-linux/tinyxml2-linux.a" "tmp/System-linux/System-linux.a" "tmp/RLSimion-Common-linux/RLSimion-Common-linux.a" -l"Cntk.Core-2.5.1" -l"Cntk.Math-2.5.1" -l"Cntk.PerformanceProfiler-2.5.1" -l"iomp5" -l"mklml_intel" -l"pthread" -Wl,-L"RLSimion/CNTKWrapper/../../debug" -Wl,-L"RLSimion/CNTKWrapper/../../bin" -Wl,-L"tmp/CNTKWrapper-linux/" -Wl,-rpath='${ORIGIN}'
echo ...Finished

echo Compiling RLSimion-Lib-linux...
mkdir tmp/RLSimion-Lib-linux
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/actor-cacla.cpp -o tmp/RLSimion-Lib-linux/actor-cacla.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/actor-critic-inac.cpp -o tmp/RLSimion-Lib-linux/actor-critic-inac.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/actor-critic-offpac.cpp -o tmp/RLSimion-Lib-linux/actor-critic-offpac.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/actor-critic-opdac.cpp -o tmp/RLSimion-Lib-linux/actor-critic-opdac.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/actor-critic.cpp -o tmp/RLSimion-Lib-linux/actor-critic.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/actor-regular.cpp -o tmp/RLSimion-Lib-linux/actor-regular.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/actor.cpp -o tmp/RLSimion-Lib-linux/actor.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/app.cpp -o tmp/RLSimion-Lib-linux/app.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/CNTKWrapperClient.cpp -o tmp/RLSimion-Lib-linux/CNTKWrapperClient.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/config.cpp -o tmp/RLSimion-Lib-linux/config.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/controller.cpp -o tmp/RLSimion-Lib-linux/controller.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/critic-td-lambda.cpp -o tmp/RLSimion-Lib-linux/critic-td-lambda.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/critic-tdc-lambda.cpp -o tmp/RLSimion-Lib-linux/critic-tdc-lambda.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/critic-true-online-td-lambda.cpp -o tmp/RLSimion-Lib-linux/critic-true-online-td-lambda.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/critic.cpp -o tmp/RLSimion-Lib-linux/critic.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/DDPG.cpp -o tmp/RLSimion-Lib-linux/DDPG.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/deep-vfa-policy.cpp -o tmp/RLSimion-Lib-linux/deep-vfa-policy.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/deferred-load.cpp -o tmp/RLSimion-Lib-linux/deferred-load.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/DQN.cpp -o tmp/RLSimion-Lib-linux/DQN.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/etraces.cpp -o tmp/RLSimion-Lib-linux/etraces.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/experience-replay.cpp -o tmp/RLSimion-Lib-linux/experience-replay.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/experiment.cpp -o tmp/RLSimion-Lib-linux/experiment.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/featuremap-discrete.cpp -o tmp/RLSimion-Lib-linux/featuremap-discrete.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/featuremap-rbfgrid.cpp -o tmp/RLSimion-Lib-linux/featuremap-rbfgrid.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/featuremap-tilecoding.cpp -o tmp/RLSimion-Lib-linux/featuremap-tilecoding.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/featuremap.cpp -o tmp/RLSimion-Lib-linux/featuremap.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/features.cpp -o tmp/RLSimion-Lib-linux/features.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/function-sampler.cpp -o tmp/RLSimion-Lib-linux/function-sampler.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/logger-functions.cpp -o tmp/RLSimion-Lib-linux/logger-functions.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/logger.cpp -o tmp/RLSimion-Lib-linux/logger.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/mem-block.cpp -o tmp/RLSimion-Lib-linux/mem-block.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/mem-buffer.cpp -o tmp/RLSimion-Lib-linux/mem-buffer.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/mem-pool.cpp -o tmp/RLSimion-Lib-linux/mem-pool.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/noise.cpp -o tmp/RLSimion-Lib-linux/noise.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/parameters-numeric.cpp -o tmp/RLSimion-Lib-linux/parameters-numeric.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/parameters.cpp -o tmp/RLSimion-Lib-linux/parameters.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/policy-learner.cpp -o tmp/RLSimion-Lib-linux/policy-learner.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/q-learners.cpp -o tmp/RLSimion-Lib-linux/q-learners.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/reward.cpp -o tmp/RLSimion-Lib-linux/reward.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/run-time-requirements.cpp -o tmp/RLSimion-Lib-linux/run-time-requirements.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/sample-file.cpp -o tmp/RLSimion-Lib-linux/sample-file.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/simgod.cpp -o tmp/RLSimion-Lib-linux/simgod.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/simion.cpp -o tmp/RLSimion-Lib-linux/simion.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/single-dimension-grid.cpp -o tmp/RLSimion-Lib-linux/single-dimension-grid.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/stats.cpp -o tmp/RLSimion-Lib-linux/stats.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/utils.cpp -o tmp/RLSimion-Lib-linux/utils.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/vfa-policy.cpp -o tmp/RLSimion-Lib-linux/vfa-policy.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/vfa.cpp -o tmp/RLSimion-Lib-linux/vfa.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/worlds/aux-rewards.cpp -o tmp/RLSimion-Lib-linux/aux-rewards.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/worlds/balancingpole.cpp -o tmp/RLSimion-Lib-linux/balancingpole.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/worlds/BulletBody.cpp -o tmp/RLSimion-Lib-linux/BulletBody.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/worlds/BulletPhysics.cpp -o tmp/RLSimion-Lib-linux/BulletPhysics.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/worlds/double-pendulum.cpp -o tmp/RLSimion-Lib-linux/double-pendulum.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/worlds/FAST.cpp -o tmp/RLSimion-Lib-linux/FAST.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/worlds/mountaincar.cpp -o tmp/RLSimion-Lib-linux/mountaincar.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/worlds/pitchcontrol.cpp -o tmp/RLSimion-Lib-linux/pitchcontrol.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/worlds/pull-box-1.cpp -o tmp/RLSimion-Lib-linux/pull-box-1.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/worlds/pull-box-2.cpp -o tmp/RLSimion-Lib-linux/pull-box-2.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/worlds/push-box-1.cpp -o tmp/RLSimion-Lib-linux/push-box-1.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/worlds/push-box-2.cpp -o tmp/RLSimion-Lib-linux/push-box-2.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/worlds/raincar.cpp -o tmp/RLSimion-Lib-linux/raincar.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/worlds/robot-control.cpp -o tmp/RLSimion-Lib-linux/robot-control.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/worlds/Robot.cpp -o tmp/RLSimion-Lib-linux/Robot.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/worlds/Rope.cpp -o tmp/RLSimion-Lib-linux/Rope.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/worlds/setpoint.cpp -o tmp/RLSimion-Lib-linux/setpoint.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/worlds/swinguppendulum.cpp -o tmp/RLSimion-Lib-linux/swinguppendulum.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/worlds/templatedConfigFile.cpp -o tmp/RLSimion-Lib-linux/templatedConfigFile.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/worlds/underwatervehicle.cpp -o tmp/RLSimion-Lib-linux/underwatervehicle.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/worlds/windturbine.cpp -o tmp/RLSimion-Lib-linux/windturbine.o
g++ -c -fPIC -w -x c++ -std=c++11 RLSimion/Lib/worlds/world.cpp -o tmp/RLSimion-Lib-linux/world.o
ar rcs tmp/RLSimion-Lib-linux/RLSimion-Lib-linux.a tmp/RLSimion-Lib-linux/*.o 
echo ...Finished

echo Compiling RLSimion-linux...
mkdir tmp/RLSimion-linux
g++ -w -x c++ -std=c++11 RLSimion/App/app-main.cpp -o tmp/RLSimion-linux/app-main.o
g++ -o tmp/RLSimion-linux/RLSimion-linux.exe tmp/RLSimion-linux/*.o -Wl,--no-undefined  "tmp/Bullet3-linux/Bullet3-linux.a" "tmp/glew2-linux/glew2-linux.a" "tmp/SOIL-linux/SOIL-linux.a" "tmp/tinyxml2-linux/tinyxml2-linux.a" "tmp/OpenGLRenderer-linux/OpenGLRenderer-linux.a" "tmp/System-linux/System-linux.a" "tmp/CNTKWrapper-linux/CNTKWrapper-linux.so" "tmp/RLSimion-Common-linux/RLSimion-Common-linux.a" "tmp/RLSimion-Lib-linux/RLSimion-Lib-linux.a" -l"GL" -l"X11" -l"GLU" -l"dl"
echo ...Finished

