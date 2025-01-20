Configure Project:
in VS -> top lefz -> Git -> Clone Repo... 
    URL: https://github.com/microsoft/vcpkg.git
    Path = "...FH_BIF5_CGRMR_OpenGL-Project\OpenGL-SnakeGame\include"

in CMD in "...FH_BIF5_CGRMR_OpenGL-Project\OpenGL-SnakeGame\include"
    ./bootstrap-vcpkg.bat
    .\vcpkg install glfw3 glad glm
    .\vcpkg integrate install

Right-click your project in Solution Explorer > Properties.
Under VC++ Directories, add the include path to Include Directories and the lib path to Library Directories.

Should now be Runable
