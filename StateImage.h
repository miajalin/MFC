// StateImage.h
//	mia @2021

#pragma once

//#ifndef _STATE_IMAGE_H_
//#define _STATE_IMAGE_H_

class CStateImage : public CImage
{
protected:
	UINT	m_nState;
	UINT	m_ImgCX;
	COLORREF m_crBkgnd;

public:
	struct SCALEIMAGELISTINFO {

		__in UINT nScaleImgW;
		__in UINT nScaleImgH;
		__in bool		bMask;
		__in COLORREF	crMask;
		__in HDC		hDestDC;
	};
	static bool ScaleImageToList(__in LPCTSTR pszImgFileName, 
									 __in UINT nImgInFile, 
									 __in SCALEIMAGELISTINFO imgInfo,
									 __out CImageList* pList);

public:
	CStateImage();
	virtual ~CStateImage();

	enum {	SIC_NONE = CLR_NONE,		/* No Background */
			SIC_DEFAULT = CLR_DEFAULT	/* Take Pixel(0,0) as Background Color */ };
	HRESULT Load(LPCTSTR pszFileName, UINT StateCount, COLORREF crBk = SIC_NONE);
	BOOL	SetTransparentColor(COLORREF crBk);	// SIC_NONE, SIC_DEFAULT or color

	/* Draw according to background mode */
	BOOL StretchBlt(HDC hDestDC, 
					int xDest, int yDest, int nDestWidth, int nDestHeight,
					UINT StateIdx);

	BOOL TransparentBlt(HDC hDestDC, 
					int xDest, int yDest, int nDestWidth, int nDestHeight,
					UINT StateIdx,
					UINT crTransparent = 0xFF000000);

	UINT ImageWidth()		{ return m_ImgCX; }
	UINT ImageHeight()		{ return CImage::GetHeight(); }
	UINT ImageListWidth()	{ return CImage::GetWidth(); }
	UINT ImageListHeight()	{ return CImage::GetHeight(); }
};

//#endif // _STATE_IMAGE_H_
