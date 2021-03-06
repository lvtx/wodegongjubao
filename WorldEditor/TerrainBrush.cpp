#include "TerrainBrush.h"
#include "RenderSystem.h"

CTerrainBrush::CTerrainBrush() :
m_fX(0),
m_fY(0),
m_fStrength(0.5f),
m_fSize(0.5f),
m_fHardness(0.5f),
m_eBrushType(BRUSH_TYPE_TERRAIN_HEIGHT),
m_eBrushHeightType(BHT_NORMAL),
m_nTileID(0),
m_uAtt(0),
m_uBaseVertexIndex(0),
m_fHeightMin(-100),
m_fHeightMax(100),
m_color(0xFFFFFFFF),
CTerrainDecal()
{
	UpdateTexTrans();
}

void CTerrainBrush::SetPos(float fX, float fY)
{
	m_fX = fX;
	m_fY = fY;
	m_uBaseVertexIndex = GetBaseVertexIndex(fX, fY);
	UpdateTexTrans();
}

void CTerrainBrush::SetSize(float fSize)
{
	m_fSize = fSize;
	UpdateTexTrans();
}

void CTerrainBrush::SetTileID(int nTileID)
{
	m_nTileID = nTileID;
}

int CTerrainBrush::GetTileID(int nX, int nY)const
{
	return m_nTileID;
}

void CTerrainBrush::SetAtt(unsigned char uAtt)
{
	m_uAtt = uAtt;
}

unsigned char CTerrainBrush::GetAtt()const
{
	return m_uAtt;
}

void CTerrainBrush::setHeightRang(float fMin, float fMax)
{
	m_fHeightMin = fMin;
	m_fHeightMax = fMax;
}

float CTerrainBrush::getHeightMin()const
{
	return m_fHeightMin;
}

float CTerrainBrush::getHeightMax()const
{
	return m_fHeightMax;
}

Color32 CTerrainBrush::getColor()const
{
	return m_color;
}

void CTerrainBrush::setColor(Color32 color)
{
	m_color = color;
}

void CTerrainBrush::UpdateTexTrans()
{
	/********************************************************************************
	顶点纹理坐标	＝（顶点纹理坐标偏移处理）×缩放比
					＝（顶点纹理坐标－贴花中心坐标)×缩放比+０.５纹理偏移
					＝顶点纹理坐标×缩放比－贴花中心坐标×缩放比+０.５纹理偏移
	顶点纹理Ｖ		＝１－（顶点纹理Ｖ×缩放比－贴花中心Ｖ×缩放比+０.５纹理偏移）
	********************************************************************************/

	// 缩放比＝半纹理／半径／ＳＩＺＥ
	float fZoom = 0.5f/(m_fRadius*m_fSize);

	// 顶点纹理Ｕ＝顶点纹理Ｕ×缩放比
	// 顶点纹理Ｖ＝－顶点纹理Ｖ×缩放比

	// －贴花中心Ｕ×缩放比＋０.５纹理偏移
	float fPosX = 0.5f-m_fX*fZoom;
	// １－（－贴花中心Ｖ×缩放比＋０.５纹理偏移）
	float fPosY = 0.5f+m_fY*fZoom;
	//
	m_mRandiusTexTrans = Matrix(
		fZoom,0,fPosX,0,
		0,-fZoom,fPosY,0,
		0,0,0,0,
		0,0,0,0);

	fZoom = 0.5f/(m_fRadius*m_fSize*m_fHardness);
	fPosX = 0.5f-m_fX*fZoom;
	fPosY = 0.5f+m_fY*fZoom;
	m_mHardnessTexTrans = Matrix(
		fZoom,0,fPosX,0,
		0,-fZoom,fPosY,0,
		0,0,0,0,
		0,0,0,0);
}

void CTerrainBrush::Render()
{
	if (IsVisible()&&setIB())
	{
		CRenderSystem& R = GetRenderSystem();
		R.SetSamplerAddressUV(0,ADDRESS_BORDER,ADDRESS_BORDER);
		// 打开纹理矩阵
		if(R.prepareMaterial("BrushRandius"))
		{
			R.setTextureMatrix(0,TTF_COUNT2,m_mRandiusTexTrans);
			Draw(m_uBaseVertexIndex);
		}
		//
		if(R.prepareMaterial("BrushHardness"))
		{
			R.setTextureMatrix(0,TTF_COUNT2,m_mHardnessTexTrans);
			Draw(m_uBaseVertexIndex);
		}
		//
		R.setTextureMatrix(0,TTF_DISABLE);
		R.SetSamplerAddressUV(0,ADDRESS_WRAP,ADDRESS_WRAP);
	}
}