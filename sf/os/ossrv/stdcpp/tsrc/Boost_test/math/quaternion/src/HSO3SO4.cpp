// test file for HSO3.hpp and HSO4.hpp

//  (C) Copyright Hubert Holin 2001.
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)


#include <iostream>

#include <boost/math/quaternion.hpp>

#include "HSO3.hpp"
#include "HSO4.hpp"


const int    number_of_intervals = 3;

const float    pi = ::std::atan(1.0f)*4;


//
//    Test of quaternion and R^3 rotation relationship
//

void    test_SO3_spherical()
{
    ::std::cout << "Testing spherical:" << ::std::endl;
    ::std::cout << ::std::endl;
    
    const float    rho = 1.0f;
    
    float        theta;
    float        phi1;
    float        phi2;
    
    for    (int idxphi2 = 0; idxphi2 <= number_of_intervals; idxphi2++)
    {
        phi2 = (-pi/2)+(idxphi2*pi)/number_of_intervals;
        
        for    (int idxphi1 = 0; idxphi1 <= number_of_intervals; idxphi1++)
        {
            phi1 = (-pi/2)+(idxphi1*pi)/number_of_intervals;
            
            for    (int idxtheta = 0; idxtheta <= number_of_intervals; idxtheta++)
            {
                theta = -pi+(idxtheta*(2*pi))/number_of_intervals;
                
                               
                ::boost::math::quaternion<float>    q = ::boost::math::spherical(rho, theta, phi1, phi2);
                
                               
                R3_matrix<float>                    rot = quaternion_to_R3_rotation(q);
                
                
                ::boost::math::quaternion<float>    p = R3_rotation_to_quaternion(rot, &q);
                
               
            }
        }
    }
    
    ::std::cout << ::std::endl;
}

    
void    test_SO3_semipolar()
{
    ::std::cout << "Testing semipolar:" << ::std::endl;
    ::std::cout << ::std::endl;
    
    const float    rho = 1.0f;
    
    float        alpha;
    float        theta1;
    float        theta2;
    
    for    (int idxalpha = 0; idxalpha <= number_of_intervals; idxalpha++)
    {
        alpha = (idxalpha*(pi/2))/number_of_intervals;
        
        for    (int idxtheta1 = 0; idxtheta1 <= number_of_intervals; idxtheta1++)
        {
            theta1 = -pi+(idxtheta1*(2*pi))/number_of_intervals;
            
            for    (int idxtheta2 = 0; idxtheta2 <= number_of_intervals; idxtheta2++)
            {
                theta2 = -pi+(idxtheta2*(2*pi))/number_of_intervals;
               
                
                ::boost::math::quaternion<float>    q = ::boost::math::semipolar(rho, alpha, theta1, theta2);
                
                                
                R3_matrix<float>                    rot = quaternion_to_R3_rotation(q);
              
                
                ::boost::math::quaternion<float>    p = R3_rotation_to_quaternion(rot, &q);
                
               
            }
        }
    }
    
    ::std::cout << ::std::endl;
}

    
void    test_SO3_multipolar()
{
    ::std::cout << "Testing multipolar:" << ::std::endl;
    ::std::cout << ::std::endl;
    
    float    rho1;
    float    rho2;
    
    float    theta1;
    float    theta2;
    
    for    (int idxrho = 0; idxrho <= number_of_intervals; idxrho++)
    {
        rho1 = (idxrho*1.0f)/number_of_intervals;
        rho2 = ::std::sqrt(1.0f-rho1*rho1);
        
        for    (int idxtheta1 = 0; idxtheta1 <= number_of_intervals; idxtheta1++)
        {
            theta1 = -pi+(idxtheta1*(2*pi))/number_of_intervals;
            
            for    (int idxtheta2 = 0; idxtheta2 <= number_of_intervals; idxtheta2++)
            {
                theta2 = -pi+(idxtheta2*(2*pi))/number_of_intervals;
                
                               
                ::boost::math::quaternion<float>    q = ::boost::math::multipolar(rho1, theta1, rho2, theta2);
                
                                
                R3_matrix<float>                    rot = quaternion_to_R3_rotation(q);
                
                                
                ::boost::math::quaternion<float>    p = R3_rotation_to_quaternion(rot, &q);
                
               
            }
        }
    }
    
    ::std::cout << ::std::endl;
}

    
void    test_SO3_cylindrospherical()
{
    ::std::cout << "Testing cylindrospherical:" << ::std::endl;
    ::std::cout << ::std::endl;
    
    float    t;
    
    float    radius;
    float    longitude;
    float    latitude;
    
    for    (int idxt = 0; idxt <= number_of_intervals; idxt++)
    {
        t = -1.0f+(idxt*2.0f)/number_of_intervals;
        radius = ::std::sqrt(1.0f-t*t);
        
        for    (int idxlatitude = 0; idxlatitude <= number_of_intervals; idxlatitude++)
        {
            latitude = (-pi/2)+(idxlatitude*pi)/number_of_intervals;
            
            for    (int idxlongitude = 0; idxlongitude <= number_of_intervals; idxlongitude++)
            {
                longitude = -pi+(idxlongitude*(2*pi))/number_of_intervals;
                
                              
                ::boost::math::quaternion<float>    q = ::boost::math::cylindrospherical(t, radius, longitude, latitude);
                
                             
                R3_matrix<float>                    rot = quaternion_to_R3_rotation(q);
                
              
                ::boost::math::quaternion<float>    p = R3_rotation_to_quaternion(rot, &q);
            
            }
        }
    }
    
    ::std::cout << ::std::endl;
}

    
void    test_SO3_cylindrical()
{
    ::std::cout << "Testing cylindrical:" << ::std::endl;
    ::std::cout << ::std::endl;
    
    float    r;
    float    angle;
    
    float    h1;
    float    h2;
    
    for    (int idxh2 = 0; idxh2 <= number_of_intervals; idxh2++)
    {
        h2 = -1.0f+(idxh2*2.0f)/number_of_intervals;
        
        for    (int idxh1 = 0; idxh1 <= number_of_intervals; idxh1++)
        {
            h1 = ::std::sqrt(1.0f-h2*h2)*(-1.0f+(idxh2*2.0f)/number_of_intervals);
            r = ::std::sqrt(1.0f-h1*h1-h2*h2);
            
            for    (int idxangle = 0; idxangle <= number_of_intervals; idxangle++)
            {
                angle = -pi+(idxangle*(2*pi))/number_of_intervals;
            
                
                ::boost::math::quaternion<float>    q = ::boost::math::cylindrical(r, angle, h1, h2);
                
             
                R3_matrix<float>                    rot = quaternion_to_R3_rotation(q);
                
              
                ::boost::math::quaternion<float>    p = R3_rotation_to_quaternion(rot, &q);
                
               
            }
        }
    }
    
    ::std::cout << ::std::endl;
}


void    test_SO3()
{
    ::std::cout << "Testing SO3:" << ::std::endl;
    ::std::cout << ::std::endl;
    
    test_SO3_spherical();
    
    test_SO3_semipolar();
    
    test_SO3_multipolar();
    
    test_SO3_cylindrospherical();
    
    test_SO3_cylindrical();
}


//
//    Test of quaternion and R^4 rotation relationship
//

void    test_SO4_spherical()
{
    ::std::cout << "Testing spherical:" << ::std::endl;
    ::std::cout << ::std::endl;
    
    const float    rho1 = 1.0f;
    const float    rho2 = 1.0f;
    
    float        theta1;
    float        phi11;
    float        phi21;
    
    float        theta2;
    float        phi12;
    float        phi22;
    
    for    (int idxphi21 = 0; idxphi21 <= number_of_intervals; idxphi21++)
    {
        phi21 = (-pi/2)+(idxphi21*pi)/number_of_intervals;
        
        for    (int idxphi22 = 0; idxphi22 <= number_of_intervals; idxphi22++)
        {
            phi22 = (-pi/2)+(idxphi22*pi)/number_of_intervals;
            
            for    (int idxphi11 = 0; idxphi11 <= number_of_intervals; idxphi11++)
            {
                phi11 = (-pi/2)+(idxphi11*pi)/number_of_intervals;
                
                for    (int idxphi12 = 0; idxphi12 <= number_of_intervals; idxphi12++)
                {
                    phi12 = (-pi/2)+(idxphi12*pi)/number_of_intervals;
                    
                    for    (int idxtheta1 = 0; idxtheta1 <= number_of_intervals; idxtheta1++)
                    {
                        theta1 = -pi+(idxtheta1*(2*pi))/number_of_intervals;
                        
                        for    (int idxtheta2 = 0; idxtheta2 <= number_of_intervals; idxtheta2++)
                        {
                            theta2 = -pi+(idxtheta2*(2*pi))/number_of_intervals;
                                                                                  
                            ::boost::math::quaternion<float>    p1 = ::boost::math::spherical(rho1, theta1, phi11, phi21);
                            
                            ::boost::math::quaternion<float>    q1 = ::boost::math::spherical(rho2, theta2, phi12, phi22);
                            
                                                     
                            ::std::pair< ::boost::math::quaternion<float> , ::boost::math::quaternion<float> >    pq1 =
                                ::std::make_pair(p1,q1);
                           
                            R4_matrix<float>                    rot = quaternions_to_R4_rotation(pq1);
                                                  
                            ::std::pair< ::boost::math::quaternion<float> , ::boost::math::quaternion<float> >    pq2 =
                                R4_rotation_to_quaternions(rot, &pq1);
                         
                        }
                    }
                }
            }
        }
    }
   
    
}


void    test_SO4()
{
        
    test_SO4_spherical();
}



#include <boost/test/included/test_exec_monitor.hpp>
int test_main(int,char *[])
{

    test_SO3();
    test_SO4();
    return 0;
}





