void main()
{
    // --- ZADANIE 2: Uproszczony normal mapping (efekt fali) ---
    // Pobieramy oryginalny wektor normalny wierzchołka

    vec3 localNormal = gl_Normal;

   //Modyfikowanie normalnych poprzez sinus

    localNormal.x += sin(gl_Vertex.x) * 1.2f;

    localNormal = normalize(gl_NormalMatrix * localNormal);



    // --- ZADANIE 1: Uproszczone oświetlenie Phonga (kierunkowe) ---


    //Pozycja wierzchołka z uwzględnieniem kamery(do specular)
    vec4 Position = gl_ModelViewMatrix * gl_Vertex;

    //Odczyt pozycji światła
    vec3 lightDirection = normalize(gl_LightSource[0].position.xyz);

    //kierunek światła względem kamery (do specular)
    vec3 viewDirection = normalize(-Position.xyz);
    
    // vector do specular (bez niego jakoś dziwnie wygląda)
    vec3 Spec_Vec = normalize(lightDirection + viewDirection);

    // --- Oświetlenie ---
    
    
    // Ambient 
    vec4 ambient = gl_LightSource[0].ambient * gl_FrontMaterial.ambient;
    
    // Diffuse

    //Wektor skalarny do obliczania kierunku padania światła
    float Light_Angle = max(dot(localNormal, lightDirection), 0.0);
    vec4 diffuse = gl_LightSource[0].diffuse * gl_FrontMaterial.diffuse * Light_Angle;
   

    // Specular
     //Wektor skalarny ale trochę inny
    float Light_Angle_V2 = max(dot(localNormal, Spec_Vec), 0.0);

    //Moc połysku
    float specularFactor = pow(Light_Angle_V2, gl_FrontMaterial.shininess);

    vec4 specular = gl_LightSource[0].specular * gl_FrontMaterial.specular * specularFactor;

   
    gl_FrontColor = ambient + diffuse + specular;

    // wyświetlanie powstałych zmian
    gl_Position = ftransform();
}