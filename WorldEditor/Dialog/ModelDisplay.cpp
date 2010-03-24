#include "ModelDisplay.h"
#include "RenderSystem.h"
#include "UIDialog.h"
#include "Graphics.h"
#include "Intersect.h"

CModelDisplay::CModelDisplay()
{
	// 生成摄像机的视角参数
	m_Camera.setYawAngle(PI/4);
	m_Camera.setPitchAngle(-PI/4);

	m_bShowBones		= false;
	m_bShowBounds		= false;
	m_bShowModel		= true;
	m_bShowWireframe	= false;
	m_bShowMaterial		= true;
	m_bShowParticles	= true;

	m_vEye = Vec3D(0,0,-4.0f);

	m_pModelObject=NULL;

	m_PickPlane[0].p.x = -10;
	m_PickPlane[0].p.y = 0;
	m_PickPlane[0].p.z = 0;

	m_PickPlane[1].p.x = 10;
	m_PickPlane[1].p.y = 0;
	m_PickPlane[1].p.z = 0;

	m_PickPlane[2].p.x = 10;
	m_PickPlane[2].p.y = 10;
	m_PickPlane[2].p.z = 0;

	m_PickPlane[3].p.x = -10;
	m_PickPlane[3].p.y = 10;
	m_PickPlane[3].p.z = 0;

	m_bLbuttonDrop = false;

	g_bLeftButtonDown = false;
	g_bRightButtonDown = false;
	g_bMiddleButtonDown = false;
}

CModelDisplay::~CModelDisplay()
{
}

#include "CSVFile.h"
void CModelDisplay::LoadModel(const std::string& strFilename)
{
	S_DEL(m_pModelObject);
	/*S_DEL(m_pC3DObj);
	if (GetExtension(strFilename)==".c3")
	{
		m_pC3DObj = new C3DObj();
		m_pC3DObj->Create(strFilename);
		CCsvFile csvFile;
		if (csvFile.Open("csv/3DObj.csv"))
		{
			if(csvFile.Seek("Name",strFilename))
			{
				std::string strID = csvFile.GetStr("ID");
				CCsvFile csvTextureFile;
				if (csvTextureFile.Open("csv/3DTexture.csv"))
				{
					if(csvTextureFile.Seek("ID",strID))
					{
						std::string strTexture = csvTextureFile.GetStr("Name");
						int nTex = GetRenderSystem().GetTextureMgr().RegisterTexture(strTexture, 1);
						for (uint32 i=0; i<m_pC3DObj->m_dwPhyNum; i++)
						{
							m_pC3DObj->m_phy[i]->nTex = nTex;
						}
					}
					csvTextureFile.Close();
				}
			}
			csvFile.Close();
		}
		return;
	}*/
	//if (GetExtension(strFilename) == ".m2"||GetExtension(strFilename) == ".sm"||GetExtension(strFilename) == ".c3")
	m_pModelObject = new CModelObject();
	m_pModelObject->load(strFilename);

	//m_pModelObject->SetAnim( 0 );
}
void CModelDisplay::loadComplexModel(const std::string& strFilename)
{
	if (m_pModelObject&&m_pModelObject->getModelFilename()==strFilename)
	{
		return;
	}
	S_DEL(m_pModelObject);

	m_pModelObject = new CModelComplex();
	m_pModelObject->load(strFilename);

	//pModelComplex->SetAnim( 0 );
}

void CModelDisplay::OnFrameMove( double fTime, float fElapsedTime )
{
	// 设置投影矩阵
	//m_Camera.SetProjParams(PI/4, m_width, m_height, 10.0f, 256.0f);
	// 更新视矩阵
	m_Camera.FrameMove(fElapsedTime);
	// 渲染更新
	if (m_pModelObject)
	{
		m_pModelObject->OnFrameMove(fElapsedTime);
		m_pModelObject->updateEmitters(Matrix::UNIT,fElapsedTime);
	}

	CUIDisplay::OnFrameMove(fTime, fElapsedTime);

	OnDropRect();
}

void MyMatrixPerspectiveReal(Matrix& Out,Vec3D& Eye, float width, float height, float fFar)
{
	float fXScale = 1.0f/width;
	float fYScale = 1.0f/height;

	float n = 1.0f /fFar;

	// [ -zEye*fXScale	0				xEye*fXScale	0		]
	// [ 0				-zEye*fYScale	yEye*fYScale	0		]
	// [ 0				0				1				0		]
	// [ 0				0				1				-zEye	]
	Out.Zero();
	Out._11 = -Eye.z*fXScale;
	Out._13 = -Eye.x*fXScale;
	Out._22 = -Eye.z*fYScale;
	Out._32 = -Eye.y*fYScale;
	Out._33 = fFar*n;
	Out._34 = -0*fFar*n;
	Out._43 = 1.0f;
	Out._44 = -Eye.z;
}

void CModelDisplay::OnFrameRender( double fTime, float fElapsedTime )
{
	CRenderSystem& R = GetRenderSystem();
	CShader* pShader = R.GetShaderMgr().getSharedShader();
	pShader->setFloat("g_fTime",fTime);
	pShader->setMatrix("g_mViewProj",m_Camera.GetProjXView());
	pShader->setMatrix("g_mView",m_Camera.GetViewMatrix());
	pShader->setVec3D("g_vLightDir",Vec3D(1,1,1));
	pShader->setVec3D("g_vEyePot",m_Camera.GetEyePt());
	// 设置投影矩阵&视矩阵
	//m_vEye = Vec3D(sin(fTime*0.3f), cos(fTime*0.5f),cos(fTime*0.8f)-2);
	Matrix mProj;
	float fWidth = (m_rcBoundingBox.right-m_rcBoundingBox.left)*0.01f;
	float fheight = (m_rcBoundingBox.bottom-m_rcBoundingBox.top)*.01f;
	MyMatrixPerspectiveReal(mProj, m_vEye, fWidth/fWidth, fheight/fWidth, 256.0f);
	m_Camera.SetProjParams(PI/4, (m_rcBoundingBox.right-m_rcBoundingBox.left), (m_rcBoundingBox.bottom-m_rcBoundingBox.top), 0.1f, 256.0f);
	R.setProjectionMatrix(/*mProj*/m_Camera.GetProjMatrix());
	R.setViewMatrix(m_Camera.GetViewMatrix());
	// 背屏清0
	R.ClearBuffer(true,false,0x0);
	// 设置视窗大小
	R.setViewport(m_rcBoundingBox);
	R.SetSamplerFilter(0, TEXF_LINEAR, TEXF_LINEAR, TEXF_LINEAR);
	R.SetSamplerFilter(1, TEXF_LINEAR, TEXF_LINEAR, TEXF_LINEAR);
	// 渲染天空盒
	m_SkyBox.Render();
	// Coordinate
	{
		m_MeshCoordinate.setPos(Vec3D(0,0,0));
		m_MeshCoordinate.setScale(1);
		m_MeshCoordinate.render(Vec3D(0,0,0));
	}
	// Gird
	{
		R.SetSamplerFilter(0, TEXF_LINEAR, TEXF_LINEAR, TEXF_LINEAR);
		R.SetSamplerFilter(1, TEXF_LINEAR, TEXF_LINEAR, TEXF_LINEAR);
		R.SetAlphaTestFunc(true);
		R.SetBlendFunc(true);
		R.SetDepthBufferFunc(true, true);
		R.SetLightingEnabled(false);
		R.SetTextureColorOP(0,TBOP_MODULATE);
		R.SetTextureAlphaOP(0,TBOP_MODULATE);

		R.SetCullingMode(CULL_NONE);
		Matrix mat;
		mat.scale(Vec3D(20.0f,20.0f,1.0f));
		R.setTextureMatrix(0,TTF_COUNT2,mat);

		static int nTex = R.GetTextureMgr().RegisterTexture("Data\\Textures\\grid.tga",1);
		GetGraphics().DrawTex3D(Vec3D(-10,0,-10),Vec3D(10,0,10),0xFF555555,nTex);
		R.setTextureMatrix(0,TTF_COUNT2,Matrix::UNIT);
	}
	// 渲染模型
	if (m_pModelObject)
	{
		if (m_bShowModel)
		{
			if (m_bShowMaterial)
			{
				m_pModelObject->render();
				CTexture* pSceneTexture = m_SceneEffect.getSceneTexture();
				pShader->setTexture("g_texScene",pSceneTexture);
				R.SetBlendFunc(false);
				m_pModelObject->render(MATERIAL_RENDER_BUMP,MATERIAL_RENDER_BUMP);
			}
			else
			{
				R.SetAlphaTestFunc(false);
				R.SetBlendFunc(false);
				R.SetDepthBufferFunc(true, true);
				R.SetLightingEnabled(true);
				R.SetTextureColorOP(0,TBOP_SOURCE2);
				R.SetTextureAlphaOP(0,TBOP_DISABLE);
				m_pModelObject->draw();
			}
		}
		if (!(m_bShowModel||m_bShowBones)||m_bShowWireframe)
		{
			R.SetFillMode( FILL_WIREFRAME );
			R.SetAlphaTestFunc(false);
			R.SetBlendFunc(false);
			R.SetDepthBufferFunc(true, true);
			R.SetLightingEnabled(false);
			R.SetTextureFactor(0xFF00FF00);
			R.SetTextureColorOP(0,TBOP_SOURCE1,TBS_TFACTOR);
			R.SetTextureAlphaOP(0,TBOP_DISABLE);
			m_pModelObject->draw();
			R.SetFillMode( FILL_SOLID );
		}
		if (m_bShowBounds)
		{
			m_pModelObject->m_pModelData->m_BoundMesh.Draw();
		}
		if (m_bShowBones)
		{
			m_pModelObject->DrawBones();
		}
	}

	//
	/*CGraphics& G = GetGraphics();
	R.SetAlphaTestFunc(false);
	R.SetBlendFunc(false);
	R.SetDepthBufferFunc(true, true);
	R.SetLightingEnabled(false);
	R.SetTextureColorOP(0,TBOP_SOURCE2);
	R.SetTextureAlphaOP(0,TBOP_DISABLE);
	Vec3D vNormal = m_Camera.GetViewDir().normalize();
	Matrix mat = m_Camera.GetViewMatrix();
	mat.Invert();
	//Vec3D v1=(mat*Vec4D(-1,-1,0,-vEye.z)).xyz();
	//Vec3D v2=(mat*Vec4D(1,-1,0,-vEye.z)).xyz();
	//Vec3D v3=(mat*Vec4D(1,1,0,-vEye.z)).xyz();
	//Vec3D v4=(mat*Vec4D(-1,1,0,-vEye.z)).xyz();
	Vec3D v1=mat*Vec3D(-fWidth,-fheight,0);
	Vec3D v2=mat*Vec3D(fWidth,-fheight,0);
	Vec3D v3=mat*Vec3D(fWidth,fheight,0);
	Vec3D v4=mat*Vec3D(-fWidth,fheight,0);
	for (int i=0; i<16; ++i)
	{
		Vec3D n = vNormal*(float)i;
		Color32 color = 0xFFFF0000-i*0xF0000+i*0xF00;
		G.DrawLine3D(v1+n, v2+n, color);
		G.DrawLine3D(v2+n, v3+n, color);
		G.DrawLine3D(v3+n, v4+n, color);
		G.DrawLine3D(v4+n, v1+n, color);
	}*/

	//R.SetAlphaTestFunc(false);
	//R.SetBlendFunc(false);
	//R.SetDepthBufferFunc(false, false);
	//R.SetLightingEnabled(false);
	//R.SetTextureFactor(0xFF00FF00);
	//R.SetTextureColorOP(0,TBOP_SOURCE1,TBS_TFACTOR);
	//R.SetTextureAlphaOP(0,TBOP_DISABLE);

	//R.SetFVF(VERTEX_XYZ_DIF::FVF);
	//R.DrawPrimitiveUP(VROT_LINE_STRIP,3,&m_PickPlane,sizeof(VERTEX_XYZ_DIF));


	//m_SceneEffect.RenderBloom();
	// 重置渲染状态，利于UI绘制
	R.SetupRenderState();
	// 重置视窗大小
	R.setViewport(GetParentDialog()->GetBoundingBox());
	CUIDisplay::OnFrameRender( fTime, fElapsedTime );
}



void CModelDisplay::OnDropRect()
{
	Vec3D vPos,vDir,vPick;
	float fU,fV,fDist;

	if(m_bLbuttonDrop)
	{
		RECT rc;
		GetRenderSystem().getViewport(rc);
		m_Camera.GetPickRay(vPos,vDir,m_DropRect.left,m_DropRect.top,m_rcBoundingBox);

		//if(D3DXIntersectTri( (D3DXVECTOR3*)&m_PickPlane[0].p, (D3DXVECTOR3*)&m_PickPlane[1].p,(D3DXVECTOR3*)&m_PickPlane[2].p,(D3DXVECTOR3*)&vPos,(D3DXVECTOR3*)&vDir,&fU,&fV,&fDist ))
		//{
		//	 vPick = m_PickPlane[0].p + fU*(m_PickPlane[1].p-m_PickPlane[0].p) + fV*(m_PickPlane[2].p-m_PickPlane[0].p);
		//	 m_vPickRectLeft = vPick;
		//}
		//else if(D3DXIntersectTri( (D3DXVECTOR3*)&m_PickPlane[0].p, (D3DXVECTOR3*)&m_PickPlane[2].p,(D3DXVECTOR3*)&m_PickPlane[3].p,(D3DXVECTOR3*)&vPos,(D3DXVECTOR3*)&vDir,&fU,&fV,&fDist ))
		//{
		//	vPick = m_PickPlane[0].p + fU*(m_PickPlane[2].p-m_PickPlane[0].p) + fV*(m_PickPlane[3].p-m_PickPlane[0].p);
		//	m_vPickRectLeft = vPick;
		//}
		if (!IntersectTri(m_PickPlane[0].p,m_PickPlane[1].p,m_PickPlane[2].p,vPos,vDir,m_vPickRectLeft))
		{
			IntersectTri(m_PickPlane[0].p,m_PickPlane[2].p,m_PickPlane[3].p,vPos,vDir,m_vPickRectLeft);
		}
	}
	
}

void CModelDisplay::OnMouseMove(POINT point)
{
	Vec3D vTargetPos;
	Vec3D vRayPos, vRayDir;
	m_Camera.GetPickRay( vRayPos, vRayDir, point.x, point.y,m_rcBoundingBox);
	if (IsPressed())
	{
		if (g_bRightButtonDown)
		{
			m_Camera.addMouseDelta(Vec3D(m_ptLastMousePosition.x-point.x,m_ptLastMousePosition.y-point.y,0));
		}
		else if (g_bMiddleButtonDown)
		{
			Vec3D vPos = m_Camera.getTargetPos();
			// 基于摄像机的 yaw 创建旋转矩阵
			Matrix mCameraRot;
			mCameraRot.rotationYawPitchRoll(m_Camera.getYawAngle(), m_Camera.getPitchAngle(), 0);
			vPos += mCameraRot * Vec3D(-point.x+m_ptLastMousePosition.x, point.y-m_ptLastMousePosition.y, 0)*0.001f*m_Camera.GetRadius();
			m_Camera.setTargetPos(vPos);
		}
		else if (g_bLeftButtonDown)
		{
		}

	}
	m_ptLastMousePosition=point;
}

void CModelDisplay::OnMouseWheel(POINT point,short wheelDelta)
{
	if(ContainsPoint(point))
	{
		if (wheelDelta!=0)
		{
			m_Camera.addMouseDelta(Vec3D(0,0,-wheelDelta/12.0f*m_Camera.GetRadius()));
		}
	}
}

void CModelDisplay::OnLButtonDown(POINT point)
{
	if(ContainsPoint(point))
	{
		SetPressed(true);
		SetCapture(UIGetHWND());
		g_bLeftButtonDown = true;
	}

	if(!m_bLbuttonDrop)
	{
		m_DropRect.set(0,0,0,0);
		m_bLbuttonDrop = true;
		POINT PT;
		GetCursorPos(&PT);
		::ScreenToClient(UIGetHWND(),&PT);
		
		m_DropRect.left = PT.x;
		m_DropRect.top = PT.y;
		m_DropRect.right = PT.x;
		m_DropRect.bottom = PT.y;

	}
}

void CModelDisplay::OnLButtonUp(POINT point)
{
	g_bLeftButtonDown = false;
	if (IsPressed())
	{
		ReleaseCapture();
	}

	m_bLbuttonDrop = false;

	POINT PT;
	GetCursorPos(&PT);
	::ScreenToClient(UIGetHWND(),&PT);

	m_DropRect.right = PT.x;
	m_DropRect.bottom = PT.y;

	
}

void CModelDisplay::OnRButtonDown(POINT point)
{
	if(ContainsPoint(point))
	{
		SetPressed(true);
		SetCapture(UIGetHWND());
		m_ptLastMousePosition=point;
		g_bRightButtonDown = true;
	}
}

void CModelDisplay::OnRButtonUp(POINT point)
{
	g_bRightButtonDown = false;
	if (IsPressed())
	{
		ReleaseCapture();
	}
}

void CModelDisplay::OnMButtonDown(POINT point)
{
	if(ContainsPoint(point))
	{
		SetPressed(true);
		SetCapture(UIGetHWND());
		m_ptLastMousePosition=point;
		g_bMiddleButtonDown=true;
	}

}

void CModelDisplay::OnMButtonUp(POINT point)
{
	g_bMiddleButtonDown=false;
	if (IsPressed())
	{
		ReleaseCapture();
	}
}

bool CModelDisplay::HandleMouse(UINT uMsg, POINT pt, WPARAM wParam, LPARAM lParam)
{
	if (WM_CAPTURECHANGED==uMsg)
	{
		if((HWND)lParam != UIGetHWND())
		{
			if (IsPressed())
			{
				ReleaseCapture();
			}
		}
	}
	return true;
}

void CModelDisplay::OnSize(const RECT& rc)
{
	CUIDisplay::OnSize(rc);
	m_SceneEffect.Reset(GetBoundingBox());
}

CModelObject* CModelDisplay::getModelObject()
{
	return m_pModelObject;
}

CModelData*  CModelDisplay::getModelData()
{
	if (m_pModelObject)
	{
		return m_pModelObject->m_pModelData;
	}
	return NULL;
}

AnimManager* CModelDisplay::getModelAnimManager()
{
	if (m_pModelObject)
	{
		return m_pModelObject->m_AnimMgr;
	}
	return NULL;
}