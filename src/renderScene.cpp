#include "gfxProgram.hpp"
#include "sprite.hpp"

void renderScene(gfxProgram &gfxPrg, std::vector<sprite> &allSprites){

	/* Change displayed digits */
	// Uber
	unsigned int onesDigitAnim = gfxPrg.uberPercent % 10;
	if(gfxPrg.uberPercent >= 100){
		allSprites.at(0).currentAnim = 2;
		allSprites.at(1).currentAnim = 1;
		allSprites.at(2).currentAnim = 1;
	}else if(gfxPrg.uberPercent >= 10){
		allSprites.at(0).currentAnim = (gfxPrg.uberPercent - onesDigitAnim) % 100 * 0.1 + 1;
		allSprites.at(1).currentAnim = onesDigitAnim + 1;
		allSprites.at(2).currentAnim = 0;
	}else{
		allSprites.at(0).currentAnim = onesDigitAnim + 1;
		allSprites.at(1).currentAnim = 0;
		allSprites.at(2).currentAnim = 0;
	}

	// Kritz
	onesDigitAnim = gfxPrg.kritzPercent % 10;
	if(gfxPrg.kritzPercent >= 100){
		allSprites.at(4).currentAnim = 2;
		allSprites.at(5).currentAnim = 1;
		allSprites.at(6).currentAnim = 1;
	}else if(gfxPrg.kritzPercent >= 10){
		allSprites.at(4).currentAnim = (gfxPrg.kritzPercent - onesDigitAnim) % 100 * 0.1 + 1;
		allSprites.at(5).currentAnim = onesDigitAnim + 1;
		allSprites.at(6).currentAnim = 0;
	}else{
		allSprites.at(4).currentAnim = onesDigitAnim + 1;
		allSprites.at(5).currentAnim = 0;
		allSprites.at(6).currentAnim = 0;
	}

	/* Render */
	glUniform1ui(gfxPrg.uberPercentID, gfxPrg.uberPercent);

	float texFrag[4];
	GLuint texWidth;
	GLuint texHeight;
	GLuint currentTexID;
	std::vector<vertex3D> currentBatch;

	for(unsigned int d = 0; d < allSprites.size(); d++){

		if(allSprites.at(d).sprTexture != NULL){

			allSprites.at(d).sprAnimateTex();
			allSprites.at(d).sprTexture->texGetFrameInfo(allSprites.at(d).currentAnim, allSprites.at(d).currentFrame,
			                                             texFrag[0], texFrag[1], texFrag[2], texFrag[3], currentTexID);
			texWidth = allSprites.at(d).sprTexture->texGetWidth(allSprites.at(d).currentAnim, allSprites.at(d).currentFrame);
			texHeight = allSprites.at(d).sprTexture->texGetHeight(allSprites.at(d).currentAnim, allSprites.at(d).currentFrame);

			// If the current texture ID differs from the last, render and clear the VBO
			if((gfxPrg.lastTexID != currentTexID && currentBatch.size() >= 6) || d == 4){
				glBufferData(GL_ARRAY_BUFFER, currentBatch.size() * sizeof(vertex3D), &currentBatch[0], GL_DYNAMIC_DRAW);
				glBindTexture(GL_TEXTURE_2D, gfxPrg.lastTexID);
				glDrawArrays(GL_TRIANGLES, 0, currentBatch.size());
				currentBatch.clear();
				if(d == 4){
					glUniform1ui(gfxPrg.uberPercentID, gfxPrg.kritzPercent);
				}
			}

			// Add sprite to the current batch
			allSprites.at(d).sprCreate(gfxPrg);
			allSprites.at(d).sprOffsetTexture(texFrag, texWidth, texHeight);
			gfxPrg.lastTexID = currentTexID;
			currentBatch.push_back(allSprites.at(d).vertices[0]);
			currentBatch.push_back(allSprites.at(d).vertices[1]);
			currentBatch.push_back(allSprites.at(d).vertices[3]);
			currentBatch.push_back(allSprites.at(d).vertices[3]);
			currentBatch.push_back(allSprites.at(d).vertices[2]);
			currentBatch.push_back(allSprites.at(d).vertices[0]);

		}

	}

	// Renders the final batch if necessary
	if(currentBatch.size() >= 6){
		glBufferData(GL_ARRAY_BUFFER, currentBatch.size() * sizeof(vertex3D), &currentBatch[0], GL_DYNAMIC_DRAW);
		glBindTexture(GL_TEXTURE_2D, gfxPrg.lastTexID);
		glDrawArrays(GL_TRIANGLES, 0, currentBatch.size());
		currentBatch.clear();
	}

}
