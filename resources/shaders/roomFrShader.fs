#version 330 core
   out vec4 FragColor;

   in vec3 aColor;
    in vec2 texCoord;
   // texture samplers
   uniform sampler2D texture1;

   void main()
   {
   	FragColor = texture(texture1,texCoord)*vec4(aColor,1.0);
   }