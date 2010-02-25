#include "DlgModelMaterial.h"
#include "ModelDisplay.h"

CDlgModelMaterial::CDlgModelMaterial():m_pSelectedMaterial(NULL)
{
}

void CDlgModelMaterial::OnControlRegister()
{
	RegisterControl("IDC_EDITBOX_DIFFUSE",	m_EditboxDiffuse);
	RegisterControl("IDC_EDITBOX_EMISSIVE",	m_EditboxEmissive);
	RegisterControl("IDC_EDITBOX_SPECULAR",	m_EditboxSpecular);
	RegisterControl("IDC_EDITBOX_BUMP",		m_EditboxBump);
	RegisterControl("IDC_EDITBOX_REFLECTION",	m_EditboxReflection);
	RegisterControl("IDC_EDITBOX_LIGHTMAP",	m_EditboxLightmap);
	RegisterControl("IDC_EDITBOX_EFFECT",	m_EditboxEffect);
	RegisterControl("IDC_CHECKBOX_ALPHATEST",	m_CheckboxAlphatest);
	RegisterControl("IDC_NUM_ALPHATESTVALUE",	m_NumAlphatestvalue);
	RegisterControl("IDC_CHECKBOX_BLEND",	m_CheckboxBlend);
	RegisterControl("IDC_CHECKBOX_CULL",	m_CheckboxCull);
	RegisterControl("IDC_NUM_TEXANIM_X",	m_NumTexanimX);
	RegisterControl("IDC_NUM_TEXANIM_Y",	m_NumTexanimY);
	RegisterControl("IDC_NUM_OPACITY",		m_NumOpacity);
	RegisterControl("IDC_COLOR_EMISSIVE",	m_ColorEmissive);
	RegisterControl("IDC_NUM_UVSCALE_X",	m_NumUvscaleX);
	RegisterControl("IDC_NUM_UVSCALE_Y",	m_NumUvscaleY);
	RegisterControl("IDC_LISTBOX_MATERIAL",	m_ListboxMaterial);

	RegisterControlEvent("IDC_EDITBOX_DIFFUSE",		(PEVENT)&CDlgModelMaterial::OnEditboxDiffuse);
	RegisterControlEvent("IDC_EDITBOX_EMISSIVE",	(PEVENT)&CDlgModelMaterial::OnEditboxEmissive);
	RegisterControlEvent("IDC_EDITBOX_SPECULAR",	(PEVENT)&CDlgModelMaterial::OnEditboxSpecular);
	RegisterControlEvent("IDC_EDITBOX_BUMP",		(PEVENT)&CDlgModelMaterial::OnEditboxBump);
	RegisterControlEvent("IDC_EDITBOX_REFLECTION",	(PEVENT)&CDlgModelMaterial::OnEditboxReflection);
	RegisterControlEvent("IDC_EDITBOX_LIGHTMAP",	(PEVENT)&CDlgModelMaterial::OnEditboxLightmap);
	RegisterControlEvent("IDC_EDITBOX_EFFECT",		(PEVENT)&CDlgModelMaterial::OnEditboxEffect);
	RegisterControlEvent("IDC_CHECKBOX_ALPHATEST",	(PEVENT)&CDlgModelMaterial::OnCheckboxAlphatest);
	RegisterControlEvent("IDC_NUM_ALPHATESTVALUE",	(PEVENT)&CDlgModelMaterial::OnNumAlphatestvalue);
	RegisterControlEvent("IDC_CHECKBOX_BLEND",	(PEVENT)&CDlgModelMaterial::OnCheckboxBlend);
	RegisterControlEvent("IDC_CHECKBOX_CULL",	(PEVENT)&CDlgModelMaterial::OnCheckboxCull);
	RegisterControlEvent("IDC_NUM_TEXANIM_X",	(PEVENT)&CDlgModelMaterial::OnNumTexanimX);
	RegisterControlEvent("IDC_NUM_TEXANIM_Y",	(PEVENT)&CDlgModelMaterial::OnNumTexanimY);
	RegisterControlEvent("IDC_NUM_OPACITY",		(PEVENT)&CDlgModelMaterial::OnNumOpacity);
	RegisterControlEvent("IDC_COLOR_EMISSIVE",	(PEVENT)&CDlgModelMaterial::OnColorEmissive);
	RegisterControlEvent("IDC_NUM_UVSCALE_X",	(PEVENT)&CDlgModelMaterial::OnNumUvscaleX);
	RegisterControlEvent("IDC_NUM_UVSCALE_Y",	(PEVENT)&CDlgModelMaterial::OnNumUvscaleY);
	RegisterControlEvent("IDC_LISTBOX_MATERIAL",(PEVENT)&CDlgModelMaterial::OnListboxMaterial);
}

void CDlgModelMaterial::onReset()
{
	int nSelected = m_ListboxMaterial.GetSelectedIndex();
	m_ListboxMaterial.RemoveAllItems();
	if (getModelDisplay().m_pModelObject&&getModelDisplay().m_pModelObject->m_pModelData)
	{
		size_t uCount = getModelDisplay().m_pModelObject->m_pModelData->m_mapPasses.size();
		if (uCount>0)
		{
			for (size_t i=0; i<uCount; ++i)
			{
				m_ListboxMaterial.AddItem(FormatW(L"[%d]",i));
			}
			if (nSelected>=uCount)
			{
				nSelected = uCount-1;
			}
			m_ListboxMaterial.SelectItem(nSelected);
		}
		OnListboxMaterial();
	}
}

CMaterial* CDlgModelMaterial::getSelectedMaterial()
{
	int nSelected = m_ListboxMaterial.GetSelectedIndex();
	if (getModelDisplay().m_pModelObject&&getModelDisplay().m_pModelObject->m_pModelData)
	{
		ModelRenderPass& renderPass = getModelDisplay().m_pModelObject->m_pModelData->m_mapPasses[nSelected];
		return &renderPass.material;
	}
	return NULL;
}

void CDlgModelMaterial::OnEditboxDiffuse()
{
	if (m_pSelectedMaterial==NULL){return;}
	CTextureMgr& TM=GetRenderSystem().GetTextureMgr();
	m_pSelectedMaterial->uDiffuse = TM.RegisterTexture(ws2s(m_EditboxDiffuse.GetText()));
}

void CDlgModelMaterial::OnEditboxEmissive()
{
	if (m_pSelectedMaterial==NULL){return;}
	CTextureMgr& TM=GetRenderSystem().GetTextureMgr();
	m_pSelectedMaterial->uEmissive = TM.RegisterTexture(ws2s(m_EditboxEmissive.GetText()));
}

void CDlgModelMaterial::OnEditboxSpecular()
{
	if (m_pSelectedMaterial==NULL){return;}
	CTextureMgr& TM=GetRenderSystem().GetTextureMgr();
	m_pSelectedMaterial->uSpecular = TM.RegisterTexture(ws2s(m_EditboxSpecular.GetText()));
}

void CDlgModelMaterial::OnEditboxBump()
{
	if (m_pSelectedMaterial==NULL){return;}
	CTextureMgr& TM=GetRenderSystem().GetTextureMgr();
	m_pSelectedMaterial->uBump = TM.RegisterTexture(ws2s(m_EditboxBump.GetText()));
}

void CDlgModelMaterial::OnEditboxReflection()
{
	if (m_pSelectedMaterial==NULL){return;}
	CTextureMgr& TM=GetRenderSystem().GetTextureMgr();
	m_pSelectedMaterial->uReflection = TM.RegisterTexture(ws2s(m_EditboxReflection.GetText()));
}

void CDlgModelMaterial::OnEditboxLightmap()
{
	if (m_pSelectedMaterial==NULL){return;}
	CTextureMgr& TM=GetRenderSystem().GetTextureMgr();
	m_pSelectedMaterial->uLightMap = TM.RegisterTexture(ws2s(m_EditboxLightmap.GetText()));
}

void CDlgModelMaterial::OnEditboxEffect()
{
	if (m_pSelectedMaterial==NULL){return;}
	m_pSelectedMaterial->uEffect = GetRenderSystem().GetShaderMgr().registerItem(ws2s(m_EditboxEffect.GetText()));
}

void CDlgModelMaterial::OnCheckboxAlphatest()
{
	if (m_pSelectedMaterial==NULL){return;}
	m_pSelectedMaterial->bAlphaTest = m_CheckboxAlphatest.IsChecked();
}

void CDlgModelMaterial::OnNumAlphatestvalue()
{
	if (m_pSelectedMaterial==NULL){return;}
	m_pSelectedMaterial->uAlphaTestValue = m_NumAlphatestvalue.getFloat();
}

void CDlgModelMaterial::OnCheckboxBlend()
{
	if (m_pSelectedMaterial==NULL){return;}
	m_pSelectedMaterial->bBlend = m_CheckboxBlend.IsChecked();
}

void CDlgModelMaterial::OnCheckboxCull()
{
	if (m_pSelectedMaterial==NULL){return;}
	m_pSelectedMaterial->bCull = m_CheckboxCull.IsChecked();
}

void CDlgModelMaterial::OnNumTexanimX()
{
	if (m_pSelectedMaterial==NULL){return;}
	m_pSelectedMaterial->vTexAnim.x = m_NumTexanimX.getFloat();
}

void CDlgModelMaterial::OnNumTexanimY()
{
	if (m_pSelectedMaterial==NULL){return;}
	m_pSelectedMaterial->vTexAnim.y = m_NumTexanimY.getFloat();
}

void CDlgModelMaterial::OnNumOpacity()
{
	if (m_pSelectedMaterial==NULL){return;}
	m_pSelectedMaterial->m_fOpacity = m_NumOpacity.getFloat();
}

void CDlgModelMaterial::OnColorEmissive()
{
	if (m_pSelectedMaterial==NULL){return;}
	m_pSelectedMaterial->cEmissive = m_ColorEmissive.getColor();
}

void CDlgModelMaterial::OnNumUvscaleX()
{
	if (m_pSelectedMaterial==NULL){return;}
	m_pSelectedMaterial->vUVScale.x = m_NumUvscaleX.getFloat();
}

void CDlgModelMaterial::OnNumUvscaleY()
{
	if (m_pSelectedMaterial==NULL){return;}
	m_pSelectedMaterial->vUVScale.y = m_NumUvscaleY.getFloat();
}

void CDlgModelMaterial::OnListboxMaterial()
{
	m_pSelectedMaterial=getSelectedMaterial();
	if (m_pSelectedMaterial)
	{
		CTextureMgr& TM=GetRenderSystem().GetTextureMgr();
		CShaderMgr& SM=GetRenderSystem().GetShaderMgr();

		m_EditboxDiffuse.SetText(s2ws(TM.getItemName(m_pSelectedMaterial->uDiffuse)));
		m_EditboxEmissive.SetText(s2ws(TM.getItemName(m_pSelectedMaterial->uEmissive)));
		m_EditboxSpecular.SetText(s2ws(TM.getItemName(m_pSelectedMaterial->uSpecular)));
		m_EditboxBump.SetText(s2ws(TM.getItemName(m_pSelectedMaterial->uBump)));
		m_EditboxReflection.SetText(s2ws(TM.getItemName(m_pSelectedMaterial->uReflection)));
		m_EditboxLightmap.SetText(s2ws(TM.getItemName(m_pSelectedMaterial->uLightMap)));
		m_EditboxEffect.SetText(s2ws(SM.getItemName(m_pSelectedMaterial->uEffect)));
		m_CheckboxAlphatest.SetChecked(m_pSelectedMaterial->bAlphaTest);
		m_NumAlphatestvalue.setFloat(m_pSelectedMaterial->uAlphaTestValue);
		m_CheckboxBlend.SetChecked(m_pSelectedMaterial->bBlend);
		m_CheckboxCull.SetChecked(m_pSelectedMaterial->bCull);
		m_NumTexanimX.setFloat(m_pSelectedMaterial->vTexAnim.x);
		m_NumTexanimY.setFloat(m_pSelectedMaterial->vTexAnim.y);
		m_NumOpacity.setFloat(m_pSelectedMaterial->m_fOpacity);
		m_ColorEmissive.setColor(m_pSelectedMaterial->cEmissive);
		m_NumUvscaleX.setFloat(m_pSelectedMaterial->vUVScale.x);
		m_NumUvscaleY.setFloat(m_pSelectedMaterial->vUVScale.y);
	}
}