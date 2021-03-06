/* -*-c++-*- 
 * 
 * osgART - Augmented Reality ToolKit for OpenSceneGraph
 * 
 * Copyright (C) 2005-2009 Human Interface Technology Laboratory New Zealand
 * Copyright (C) 2010-2014 Raphael Grasset, Julian Looser, Hartmut Seichter
 *
 * This library is open source and may be redistributed and/or modified under
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the osgart.org website.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * OpenSceneGraph Public License for more details.
*/

// std include
#include <fstream>

// OpenThreads include
#include <OpenThreads/ScopedLock>

// OSG include
#include <osg/Node>
#include <osg/Texture>
#include <osg/Texture2D>
#include <osg/TextureRectangle>
#include <osg/BlendFunc>
#include <osg/Notify>
#include <osg/Geometry>
#include <osg/ImageStream>

// local include
#include "osgART/CameraConfiguration"
#include "osgART/Utils"
#include "osgART/Video"
#include "osgART/VideoStream"
#include "osgART/VideoPlane"



namespace osgART {

	namespace Internal {

		class Texture2DCallback : public osg::Texture2D::SubloadCallback
		{
		public:
			
			Texture2DCallback(osg::Texture2D* texture);

			void load(const osg::Texture2D& texture, osg::State&) const;
			void subload(const osg::Texture2D& texture, osg::State&) const;

            inline float getTexCoordX() const { return (_texCoordX);}
            inline float getTexCoordY() const { return (_texCoordY);}
			
			// needed for OSG 3.0
			inline bool textureObjectValid(const osg::Texture2D& texture, osg::State& state) const
            {
				return true;
            }
			

		protected:
			
			float _texCoordX;
			float _texCoordY;

		};


		Texture2DCallback::Texture2DCallback(osg::Texture2D* texture) 
			: osg::Texture2D::SubloadCallback()
			, _texCoordX(texture->getImage()->s() / (float)equalOrGreaterPowerOfTwo((unsigned int)texture->getImage()->s()))
			, _texCoordY(texture->getImage()->t() / (float)equalOrGreaterPowerOfTwo((unsigned int)texture->getImage()->t()))
		{
			texture->setTextureSize(equalOrGreaterPowerOfTwo((unsigned int)texture->getImage()->s()),
				equalOrGreaterPowerOfTwo((unsigned int)texture->getImage()->t()));

		//	std::cout<<"texture format.."<<equalOrGreaterPowerOfTwo((unsigned int)texture->getImage()->s())<<"x"<<
		//		equalOrGreaterPowerOfTwo((unsigned int)texture->getImage()->t())<<std::endl;
			texture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
			texture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
			texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::CLAMP);
			texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::CLAMP);
			
		}

		/*virtual*/ 
		void 
		Texture2DCallback::load(const osg::Texture2D& texture, osg::State& state) const 
		{
			
			const osg::Image* _image = texture.getImage();

			glTexImage2D(GL_TEXTURE_2D, 0, 
				// hse25: internal texture format gets overwritten by the image format 
				// we need just the components - ???
				//osg::Image::computeNumComponents(_image->getInternalTextureFormat()), 
				osg::Image::computeNumComponents(_image->getPixelFormat()), 
				(float)equalOrGreaterPowerOfTwo((unsigned int)_image->s()), 
				(float)equalOrGreaterPowerOfTwo((unsigned int)_image->t()), 
				0, _image->getPixelFormat(), 
				_image->getDataType(), 0);

		}
		
		/*virtual */ 
		void
		Texture2DCallback::subload(const osg::Texture2D& texture, osg::State& state) const 
		{
			
			osg::Image* _image = const_cast<osg::Image*>(texture.getImage());
			osgART::VideoStream* vid = dynamic_cast<osgART::VideoStream*>(_image);
			if (vid) {
				//OpenThreads::ScopedLock<OpenThreads::Mutex> lock(vid->getMutex());
				texture.applyTexImage2D_subload(state, GL_TEXTURE_2D, _image, _image->s(), _image->t(), _image->getInternalTextureFormat(), 1);
			} else {
				texture.applyTexImage2D_subload(state, GL_TEXTURE_2D, _image, _image->s(), _image->t(), _image->getInternalTextureFormat(), 1);
			}
		}
    } //namespace Internal


    VideoPlane::VideoPlane(
                osg::Image* image,
                TextureMode textureMode,			// Mode to texture with
                double width,
                double height,
                int cols,							// Number of columns
                int rows)							// Number of rows);


         : VideoGeode(image,textureMode)
    {

        this->addDrawable(createVideoPlane(width, height, cols, rows));

    }

    VideoPlane::VideoPlane(const VideoPlane&, const osg::CopyOp& /*copyop = CopyOp::SHALLOW_COPY*/)
    {
    }

    /*
    VideoPlane::~VideoPlane()
    {
    }*/

    osg::Geometry* VideoPlane::createVideoPlane(
        double width,
        double height,
        int cols,							// Number of columns
        int rows)							// Number of rows

    {
        osg::Geometry* geom = new osg::Geometry();
        geom->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);

        geom->getOrCreateStateSet()->setMode(GL_CULL_FACE, GL_FALSE);

        osg::Vec2Array* verts = new osg::Vec2Array();
        geom->setVertexArray(verts);
        osg::Vec2Array* tcs = new osg::Vec2Array();
        geom->setTexCoordArray(0, tcs);
        osg::Vec4Array* colors = new osg::Vec4Array();
        colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
        geom->setColorArray(colors);
        geom->setColorBinding(osg::Geometry::BIND_OVERALL);

        double imageWidth = (double)_image->s();
        double imageHeight = (double)_image->t();

        double maxU = 1.0f, maxV = 1.0f;

        switch (_textureMode) {

            case USE_TEXTURE_RECTANGLE:
                {
                    maxU = imageWidth;
                    maxV = imageHeight;
                    osg::TextureRectangle* texture = new osg::TextureRectangle(_image);
                    texture->setDataVariance(osg::Object::DYNAMIC);
                    geom->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture);
                    break;
                }

            case USE_TEXTURE_2D:
            default:
                {
                    maxU = imageWidth / (double)osgART::equalOrGreaterPowerOfTwo((unsigned int)imageWidth);
                    maxV = imageHeight / (double)osgART::equalOrGreaterPowerOfTwo((unsigned int)imageHeight);
                    osg::Texture2D* texture = new osg::Texture2D(_image);
                    texture->setDataVariance(osg::Object::DYNAMIC);
                    texture->setSubloadCallback(new Internal::Texture2DCallback(texture));
                    geom->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture);
                    break;
                }
        }

        for (int r = 0; r < rows; r++) {
            for (int c = 0; c <= cols; c++) {

                double dr1 = (double)r / (double)rows;
                double dr2 = (double)(r+1) / (double)rows;
                double dc = (double)c / (double)cols;

                osg::Vec2d p1(dc, dr1), p2(dc, dr2);	// Normalised coordinates of distorted pixels
                osg::Vec2d u_p1 = p1, u_p2 = p2;		// Normalises coordinates of undistorted pixels (to be computed)

                // Scale to widthxheight
                u_p1[0] *= width;
                u_p1[1] *= height;
                u_p2[0] *= width;
                u_p2[1] *= height;

                verts->push_back(u_p1);
                verts->push_back(u_p2);

                tcs->push_back(osg::Vec2(dc * maxU, (1.0f - dr1) * maxV));
                tcs->push_back(osg::Vec2(dc * maxU, (1.0f - dr2) * maxV));

            }

            geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP, r * 2 * (cols+1), 2 * (cols+1)));
        }



        return geom;

    }

    UndistortedVideoPlane::UndistortedVideoPlane(
                osg::Image* image,
                TextureMode textureMode,			// Mode to texture with
                osgART::CameraConfiguration* cameraconfig,
                double width,
                double height,
                int cols,							// Number of columns
                int rows)							// Number of rows);

         : VideoPlane(image,textureMode)
    {

        this->addDrawable(createUndistortedVideoPlane(cameraconfig, width, height, cols, rows));

    }

    UndistortedVideoPlane::UndistortedVideoPlane(const UndistortedVideoPlane&, const osg::CopyOp& /*copyop = CopyOp::SHALLOW_COPY*/)
    {
    }



    osg::Geometry* UndistortedVideoPlane::createUndistortedVideoPlane(
        osgART::CameraConfiguration* cameraconfig,	// Camera Configuration object for undistortion calculation
        double width,
        double height,
        int cols,							// Number of columns
        int rows)							// Number of rows

    {

        osg::Geometry* geom = new osg::Geometry();
        geom->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);

        geom->getOrCreateStateSet()->setMode(GL_CULL_FACE, GL_FALSE);

        osg::Vec2Array* verts = new osg::Vec2Array();
        geom->setVertexArray(verts);
        osg::Vec2Array* tcs = new osg::Vec2Array();
        geom->setTexCoordArray(0, tcs);
        osg::Vec4Array* colors = new osg::Vec4Array();
        colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
        geom->setColorArray(colors);
        geom->setColorBinding(osg::Geometry::BIND_OVERALL);

        double imageWidth = (double)_image->s();
        double imageHeight = (double)_image->t();

        double maxU = 1.0f, maxV = 1.0f;

        switch (_textureMode) {

            case USE_TEXTURE_RECTANGLE:
                {
                    maxU = imageWidth;
                    maxV = imageHeight;
                    osg::TextureRectangle* texture = new osg::TextureRectangle(_image);
                    texture->setDataVariance(osg::Object::DYNAMIC);
                    geom->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture);
                    break;
                }

            case USE_TEXTURE_2D:
            default:
                {
                    maxU = imageWidth / (double)osgART::equalOrGreaterPowerOfTwo((unsigned int)imageWidth);
                    maxV = imageHeight / (double)osgART::equalOrGreaterPowerOfTwo((unsigned int)imageHeight);
                    osg::Texture2D* texture = new osg::Texture2D(_image);
                    texture->setDataVariance(osg::Object::DYNAMIC);
                    texture->setSubloadCallback(new Internal::Texture2DCallback(texture));
                    geom->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture);
                    break;
                }
        }

        for (int r = 0; r < rows; r++) {
            for (int c = 0; c <= cols; c++) {

                double dr1 = (double)r / (double)rows;
                double dr2 = (double)(r+1) / (double)rows;
                double dc = (double)c / (double)cols;

                osg::Vec2d p1(dc, dr1), p2(dc, dr2);	// Normalised coordinates of distorted pixels
                osg::Vec2d u_p1 = p1, u_p2 = p2;		// Normalises coordinates of undistorted pixels (to be computed)

                if (cameraconfig) {
                    cameraconfig->undistort(p1[0] * imageWidth, p1[1] * imageHeight, &u_p1[0], &u_p1[1]);
                    u_p1[0] /= imageWidth;
                    u_p1[1] /= imageHeight;
                    cameraconfig->undistort(p2[0] * imageWidth, p2[1] * imageHeight, &u_p2[0], &u_p2[1]);
                    u_p2[0] /= imageWidth;
                    u_p2[1] /= imageHeight;
                }

                // Scale to widthxheight
                u_p1[0] *= width;
                u_p1[1] *= height;
                u_p2[0] *= width;
                u_p2[1] *= height;

                verts->push_back(u_p1);
                verts->push_back(u_p2);

                tcs->push_back(osg::Vec2(dc * maxU, (1.0f - dr1) * maxV));
                tcs->push_back(osg::Vec2(dc * maxU, (1.0f - dr2) * maxV));

            }

            geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP, r * 2 * (cols+1), 2 * (cols+1)));
        }



        return geom;

    }
} //namespace osgART
