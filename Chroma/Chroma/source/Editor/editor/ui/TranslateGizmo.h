#ifndef CHROMA_TRANSLATEGIZMO_H
#define CHROMA_TRANSLATEGIZMO_H


#include <editor/ui/IGizmo.h>
class TranslateGizmo :	public IGizmo
{
public:
	TranslateGizmo();
	void Init() override;
private:

};

#endif // CHROMA_TRANSLATEGIZMO_H