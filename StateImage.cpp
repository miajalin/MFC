// StateImage.cpp
//	mia @2021
// ---------------------------------------------------
//	CImage can draw specific state image
// ---------------------------------------------------


#include "stdafx.h"

#include "StateImage.h"


CStateImage::CStateImage()
{
	m_nState = 0;
	m_ImgCX  = 0;
	m_crBkgnd = 0xFF000000;
}

CStateImage::~CStateImage()
{
}

HRESULT CStateImage::Load(LPCTSTR pszFileName, UINT StateCount, COLORREF crBk /*= SIC_NONE*/)
{
	ASSERT(pszFileName);
	if (NULL == pszFileName)
		return -1;
	ASSERT(StateCount);
	if (StateCount <= 0)
		return -1;

	if (FAILED(CImage::Load(pszFileName))) {
		ASSERT(0);
		return -1;
	}

	UINT nW		= (UINT)CImage::GetWidth();
	ASSERT(nW % StateCount == 0);

	m_ImgCX		= nW / StateCount;
	m_nState	= StateCount;

	SetTransparentColor(crBk);

	return 0;
}

BOOL CStateImage::SetTransparentColor(COLORREF crBk)
{
	if (IsNull())
		return FALSE;

	m_crBkgnd	= crBk;

	if (crBk == SIC_NONE)
		; // Do Nothing
	else if (crBk == SIC_DEFAULT)
	{
		m_crBkgnd = GetPixel(0,0);
		CImage::SetTransparentColor(m_crBkgnd);
	}
	else
		CImage::SetTransparentColor(crBk);

	return TRUE;
}

BOOL CStateImage::StretchBlt(
	HDC hDestDC, 
	int xDest, int yDest, int nDestWidth, int nDestHeight,
	UINT StateIdx)
{
	::SetStretchBltMode(hDestDC, HALFTONE);

	if (SIC_NONE == m_crBkgnd)
	{
		return CImage::StretchBlt(hDestDC, 
								  xDest, yDest, nDestWidth, nDestHeight,
								  m_ImgCX * StateIdx, 0, m_ImgCX, GetHeight());
	}
	else
	{
		return CImage::TransparentBlt(hDestDC, 
							  xDest, yDest, nDestWidth, nDestHeight,
							  m_ImgCX * StateIdx, 0, m_ImgCX, GetHeight(),
							  m_crBkgnd);
	}
}

BOOL CStateImage::TransparentBlt(HDC hDestDC, 
					int xDest, int yDest, int nDestWidth, int nDestHeight,
					UINT StateIdx,
					UINT crTransparent /*= 0xFF000000*/)
{
	::SetStretchBltMode(hDestDC, HALFTONE);

	COLORREF crTrans = crTransparent;
	if (0xFF000000 == crTrans)
	{
		if (SIC_DEFAULT == m_crBkgnd)
			crTrans = m_crBkgnd;
		else
			;//ASSERT(0);
	}

	return CImage::TransparentBlt(hDestDC, 
							  xDest, yDest, nDestWidth, nDestHeight,
							  m_ImgCX * StateIdx, 0, m_ImgCX, GetHeight(),
							  crTrans);
}

/*static*/ 
bool CStateImage::ScaleImageToList(
	__in LPCTSTR pszImgFileName,		// File of Images
	__in UINT nImgInFile,				// Image Count in Image File
	__in SCALEIMAGELISTINFO imgInfo,
	__out CImageList* pList				// The outcome ImageList with scaled images
)
{
	ASSERT(imgInfo.hDestDC);
	if (NULL == imgInfo.hDestDC) {
		OutErr(_T("DPI_ScaleImageToList hDestDC!"));
		return false;
	}
	
	ASSERT(pList);
	if (NULL == pList) {
		OutErr(_T("DPI_ScaleImageToList pList!"));
		return false;
	}


	//
	CStateImage sImg;
	if (FAILED(sImg.Load(pszImgFileName, nImgInFile)))
		return false;
	
	//
	UINT uFlag = ILC_COLORDDB;
	if (imgInfo.bMask)
		uFlag |= ILC_MASK;
	if (NULL == pList->GetSafeHandle())
	{
		if (FALSE == pList->Create(imgInfo.nScaleImgW, imgInfo.nScaleImgH, 
								   uFlag, 0, 0))
		{	OutErr("ImageList Create!"); return false;	}
	}
	else
	{
		int cx =-1, cy =-1;
		if (FALSE == ImageList_GetIconSize(pList->GetSafeHandle(), &cx, &cy))
			return false;
		if (cx != imgInfo.nScaleImgW || cy != imgInfo.nScaleImgH)
		{	OutErr("Scale Size!"); return false; }
	}

	//
	HDC hMemDC = CreateCompatibleDC(imgInfo.hDestDC);
	ASSERT(hMemDC);

	CRect rcImage(0, 0, imgInfo.nScaleImgW, imgInfo.nScaleImgH);
	HBITMAP hBitmap = CreateCompatibleBitmap(imgInfo.hDestDC, imgInfo.nScaleImgW, imgInfo.nScaleImgH);
	ASSERT(hBitmap);

	for (int i =0; i < nImgInFile; i++)
	{
		HGDIOBJ hOldBmp = SelectObject(hMemDC, hBitmap);
		sImg.StretchBlt(hMemDC, 0,0, imgInfo.nScaleImgW, imgInfo.nScaleImgH, i);
		SelectObject(hMemDC,hOldBmp);

		//ImageList_Add(pList->GetSafeHandle(), hBitmap, NULL);

		CBitmap bitmap;
		bitmap.Attach(hBitmap);
		pList->Add(&bitmap, imgInfo.crMask);
		bitmap.Detach();
	}

	DeleteObject(hBitmap);
	DeleteDC(hMemDC);

	return true;
}
