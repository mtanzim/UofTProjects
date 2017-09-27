			/*
			glEnable (GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			

			//personal texture blending
			//mid-high transition
			if (temp[0][1]/256 > mid_high_border || temp[1][1]/256 > mid_high_border ||temp[2][1]/256 > mid_high_border || temp[3][1]/256 > mid_high_border) {
				//enable blending
				glEnable( GL_BLEND );
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				
				//glDisable(GL_DEPTH_TEST);
				glActiveTexture(GL_TEXTURE0);
				glEnable( GL_TEXTURE_2D );
				glActiveTexture(GL_TEXTURE1);
				glEnable( GL_TEXTURE_2D );	
				glBindTexture( GL_TEXTURE_2D, tex0);
				glColor4f(1.0f, 1.0f, 1.0f, 0.5f);

				glBindTexture( GL_TEXTURE_2D, tex1);
				glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
				
				//glEnable(GL_DEPTH_TEST);
				//high terrain
				if (temp[0][1]/256 > high_border || temp[1][1]/256 > high_border ||temp[2][1]/256 > high_border || temp[3][1]/256 > high_border) {
					glDisable( GL_BLEND );
					glActiveTexture (GL_TEXTURE0);
					glEnable (GL_TEXTURE_2D);
					glBindTexture( GL_TEXTURE_2D, tex1);
					glActiveTexture (GL_TEXTURE1);
					glDisable (GL_TEXTURE_2D);
				}
			} 

			//low terrain
			else{
				glDisable( GL_BLEND );
				glActiveTexture (GL_TEXTURE0);
				glEnable (GL_TEXTURE_2D);
				glBindTexture( GL_TEXTURE_2D, tex0);
				glActiveTexture (GL_TEXTURE1);
				glDisable (GL_TEXTURE_2D);
				
			}
			*/
			
			
			//basic 1 texture
			/*
			glActiveTexture (GL_TEXTURE0);
			glEnable (GL_TEXTURE_2D);
			glBindTexture( GL_TEXTURE_2D, tex0);
			*/
