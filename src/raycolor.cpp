#include "raycolor.h"
#include "first_hit.h"
#include "blinn_phong_shading.h"
#include "reflect.h"
#include <cmath>

bool raycolor(
  const Ray & ray, 
  const double min_t,
  const std::vector< std::shared_ptr<Object> > & objects,
  const std::vector< std::shared_ptr<Light> > & lights,
  const int num_recursive_calls,
  Eigen::Vector3d & rgb)
{
  // Shoot a ray into a lit scene and collect color information.
  //
  // Inputs:
  //   ray  ray along which to search
  //   min_t  minimum t value to consider (for viewing rays, this is typically at
  //     least the _parametric_ distance of the image plane to the camera)
  //   objects  list of objects (shapes) in the scene
  //   lights  list of lights in the scene
  //   num_recursive_calls  how many times has raycolor been called already
  // Outputs:
  //   rgb  collected color 
  // Returns true iff a hit was found
  ////////////////////////////////////////////////////////////////////////////

  // Ambient, Diffuse, Specular, Mirror Color
  // Eigen::Vector3d ka,kd,ks,km;
  // Phong exponent
  // double phong_exponent;

  int hit_id;
  double t;
  Eigen::Vector3d n;
  bool result = first_hit(ray,min_t,objects,hit_id,t,n);

  if(result != false) {
      
    //mirror reflection
    Eigen::Vector3d mirror = (*(*objects[hit_id]).material).km;
    Eigen::Vector3d mirror_direction = reflect(ray.direction.normalized(), n);
    Eigen::Vector3d intersection_point = ray.origin + ray.direction * t;
    Ray reflection_ray = Ray();
    reflection_ray.origin = intersection_point; 
    reflection_ray.direction = mirror_direction;

    rgb += blinn_phong_shading(ray, hit_id, t, n, objects, lights);
    
    int reflection_hit_id;
    double reflection_t;
    Eigen::Vector3d reflection_n;
    Eigen::Vector3d collected_color = Eigen::Vector3d(0,0,0);
    if(num_recursive_calls <= 2){//prevent infinite recursion, need to choose a suitable recursion depth
      raycolor(reflection_ray, 0.000000000001 /*prevent accidental intercept with starting object*/, objects, lights, num_recursive_calls+1, collected_color);
      rgb = rgb + (mirror.array() * collected_color.array()).matrix();
    }
  }


  return result;
  ////////////////////////////////////////////////////////////////////////////
}
