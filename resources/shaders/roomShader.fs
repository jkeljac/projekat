#version 330 core
   out vec4 FragColor;

   in vec3 aColor;
    in vec2 texCoord;
   // texture samplers
   uniform sampler2D floor_texture;

   void main()
   {
   	FragColor = texture(floor_texture,texCoord)*vec4(aColor,1.0);
   }