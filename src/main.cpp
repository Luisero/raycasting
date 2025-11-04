#include "../include/Color.hpp"
#include "../include/Point.hpp"
#include "../include/Ray.hpp"
#include "../include/Sphere.hpp"
#include "../include/Vector.hpp"
#include "../include/Plane.hpp"
#include "../include/MirrorSphere.hpp"
#include <algorithm>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory.h>
#include <memory>

// --- CONSTANTS ---
const float windowWidth = 2.f, windowHeight = 1.5f;
const int numCols = 2.f * 300, numRows = 1.5 * 300;
float Dx = windowWidth / numCols;
float Dy = windowHeight / numRows;
float viewplaneDistance = 10;
const int FRAMES_AMOUNT = 1;

Point observerPosition(0, 0, 0, 1);

Point lightPosition(-1.f, 2.0f, viewplaneDistance, 1.0f);
Color lightIntensity(255, 255, 255);
Color ambientLightIntensity(80,80,80);

void convertDisplayToWindow(int display_x, int display_y, float &ndc_x,
                              float &ndc_y)
{
  // Adds 0.5 to get the center of the pixel
  ndc_x = -windowWidth / 2.0f + Dx / 2.0f + display_x * Dx;
  ndc_y = windowHeight / 2.0f - Dy / 2.0f - display_y * Dy;
}

Object *getIntersectedObject(Ray ray,
                             const std::vector<std::unique_ptr<Object>> &objects,
                             float &closest_t)
{
  closest_t = -1.0f;
  Object *closestObject = nullptr;

  for (const auto &object : objects)
  {
    if (object == nullptr)
    {
      continue;
    }
    float current_t = object->intersect(ray);

    if (current_t > 0.001f) // Epsilon to avoid self-intersection
    {
      if (closest_t == -1.0f || current_t < closest_t)
      {
        closest_t = current_t;
        closestObject = object.get();
      }
    }
  }
  return closestObject;
}

int main()
{
  float time = 0;
  // --- MATERIALS ---
  Material matOrange(Color(10, 5, 2),
                       Color(200, 100, 50),
                       Color(255, 255, 255),
                       128.0f);
  Material matPink(Color(10, 5, 2),
                       Color(255,141,161),
                       Color(255, 255, 255),
                       128.0f);
  Material matRed(Color(10, 5, 2),
                       Color(255,10,20),
                       Color(255, 255, 255),
                       128.0f);
  Material matFloor(Color(40, 90, 50),
                    Color(35, 196, 12),
                    Color(255, 255, 255),
                    255.f);
  Material matWall(Color(10, 10, 20),
                   Color(35, 100, 196),
                   Color(255, 255, 255),
                   99999.f);
  Material matMirror(Color(0, 0, 0),
                     Color(0, 0, 0),
                     Color(0, 0, 0),
                     128.f);

  float sphereRadius = .2f;
  Point defaultCenter(0.f, 0.f, -viewplaneDistance + sphereRadius, 1.f);

  std::vector<std::unique_ptr<Object>> objects;

  // --- SCENE OBJECTS ---
  // 1. Orange Sphere
  objects.push_back(std::make_unique<Sphere>(defaultCenter, sphereRadius, matOrange));
  
  // 2. Pink Sphere
  objects.push_back(std::make_unique<Sphere>(
      defaultCenter + Vector4(0.5f, 0.2f, -1.0f, 0.f),
      sphereRadius,
      matPink));
  
  // 3. Red Sphere
  objects.push_back(std::make_unique<Sphere>(
        defaultCenter + Vector4(-0.1f, 0.2f, viewplaneDistance *2, 0.f),
        sphereRadius*2,
        matRed));
  
  // 4. Mirror Sphere
  objects.push_back(std::make_unique<MirrorSphere>(
      defaultCenter + Vector4(-0.5, .5f, 0.f, 0.f), sphereRadius,
      matMirror));

  // 5. Floor
  Point floorPoint(0.f, -sphereRadius * 2.f, 0.f, 1.f);
  Vector4 floorNormal(0.f, 1.f, 0.f, 0.f);
  objects.push_back(std::make_unique<Plane>(floorPoint, floorNormal, matFloor));

  // 6. Wall
  Point wallPoint(0.f, 0.f, -viewplaneDistance *2, 1.f);
  Vector4 wallNormal(0, 0, 1, 0);
  objects.push_back(std::make_unique<Plane>(wallPoint, wallNormal, matWall));


  // --- MAIN LOOP ---
  for (int i = 0; i < FRAMES_AMOUNT; i++)
  {
    time += 0.1;
    
    std::string frametitle = "image";
    frametitle.append(std::to_string(i));
    frametitle.append(".ppm");
    std::ofstream image(frametitle);

    if (image.is_open())
    {
      image << "P3\n";
      image << numCols << " " << numRows << "\n";
      image << 255 << "\n";

      // --- Animation Step ---
      for (int j = 0; j < objects.size(); j++)
      {
        if (objects[j] == nullptr)
        {
          continue;
        }

        // Animate only objects that are Spheres (or derived from Sphere)
        if (Sphere *sphere = dynamic_cast<Sphere *>(objects[j].get()))
        {
          sphere->center.y += sin(time + j) / 100;
          sphere->center.z += cos(time + j) / 10;
        }
      }
      // Animate light
      lightPosition.y -= 0.001f;
      lightPosition.x += cos(time / 4) / 100;

      // --- Render Step ---
      for (int l = 0; l < numRows; l++)
      {
        for (int c = 0; c < numCols; c++)
        {
          float x, y;
          convertDisplayToWindow(c, l, x, y);
          Vector4 d(x - observerPosition.x, y - observerPosition.y, -viewplaneDistance, 0);

          Ray ray(d.normalize(), observerPosition);

          float closest_t;
          Object *closestObject = getIntersectedObject(ray, objects, closest_t);

          if (closestObject == nullptr)
          {
            image << "10 50 200 "; // Background color
          }
          else
          {
            Point P = ray.origin + (ray.dir * closest_t);
            Vector4 N = closestObject->getNormal(P);
            N.normalize();

            // Check if the object is a mirror
            if (MirrorSphere *v = dynamic_cast<MirrorSphere *>(closestObject))
            {
              // --- Reflection Logic ---
              Vector4 D = ray.dir.normalize();
              float d_dot_n = D.dot(N);
              Vector4 reflectedDirection = D - (N * (2.0f * d_dot_n));
              reflectedDirection.normalize();

              Point reflectedOrigin = P + (N * 0.001f);
              Ray reflected_ray(reflectedDirection, reflectedOrigin);

              float reflected_t;
              Object *reflectedObject = getIntersectedObject(reflected_ray, objects, reflected_t);

              if (reflectedObject == nullptr)
              {
                image << "10 50 200 "; // Reflected background
              }
              else
              {
                // Use the diffuse color of the reflected object
                Color reflectedColor = reflectedObject->material.Kd;
                reflectedColor.clamp();
                image << reflectedColor.r << " " << reflectedColor.g << " " << reflectedColor.b << " ";
              }
            }
            else
            {
              // --- Phong Shading Logic ---
              Material mat = closestObject->material;
              Color ambientColor = mat.Ka * ambientLightIntensity;

              Vector4 lightVector = (lightPosition - P);
              float distanceToLight = lightVector.lenght();
              Vector4 lightDirection = lightVector.normalize();

              // --- Shadow Check ---
              Point shadowRayOrigin = P + (N * 0.001f);
              Ray shadowRay(lightDirection, shadowRayOrigin);

              float shadow_t;
              Object *obstructingObject = getIntersectedObject(shadowRay, objects, shadow_t);
              
              // The point is in shadow if:
              // 1. The shadow ray hit something.
              // 2. The object hit is *between* the point and the light.
              bool inShadow = (obstructingObject != nullptr) && (shadow_t < distanceToLight);
              
              Color diffuseColor(0, 0, 0);
              Color specularColor(0, 0, 0);

              if (!inShadow)
              {
                Vector4 V = (observerPosition - P).normalize();
                Vector4 R = (N * (2.0f * N.dot(lightDirection))) - lightDirection;

                float diffuseFactor = std::max(0.0f, N.dot(lightDirection));
                if (diffuseFactor > 0)
                {
                  diffuseColor = (mat.Kd * lightIntensity) * diffuseFactor;
                }

                float specularFactor =
                    std::pow(std::max(0.0f, V.dot(R.normalize())), mat.shininess);
                if (specularFactor > 0)
                {
                  specularColor = (mat.Ks * lightIntensity) * specularFactor;
                }
              }
              
              // Final color is ambient + (diffuse + specular if not in shadow)
              Color finalColor = ambientColor + diffuseColor + specularColor;
              finalColor.clamp();

              image << finalColor.r << " " << finalColor.g << " " << finalColor.b
                    << " ";
            }
          }
        }
        image << "\n";
      }
      image.close();
    }
  }
  return 0;
}