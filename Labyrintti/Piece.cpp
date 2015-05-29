#include "Piece.h"

Piece::Piece(Julkinen::Koordinaatti coord, Julkinen::PalaTyyppi type, unsigned int rotation) :
Location(coord), Type(type), Rotation(rotation), 
SpecialType(Julkinen::ErikoispalaTyyppi()), Target(Julkinen::Koordinaatti()), Item('x')
{}
Piece::Piece(const Piece &org) :
Location(org.Location), Type(org.Type), Rotation(org.Rotation), 
SpecialType(org.SpecialType), Target(org.Target), Item(org.Item)
{}
Piece::~Piece()
{}
Piece& Piece::operator=(const Piece &source)
{
	if (this != &source)
	{
		Location = source.Location;
		Type = source.Type;
		Rotation = source.Rotation;
		SpecialType = source.SpecialType;
		Target = source.Target;
		Item = source.Item;
	}
	return *this;
}

Julkinen::Koordinaatti Piece::getLocation() const{
	return Location;
}
Julkinen::PalaTyyppi Piece::getType() const{
	return Type;
}
unsigned int Piece::getRotation() const{
	return Rotation;
}
Julkinen::ErikoispalaTyyppi Piece::getSpecialType() const{
	return SpecialType;
}
Julkinen::Koordinaatti Piece::getTarget() const{
	return Target;
}
char Piece::getItem() const{
	return Item;
}
void Piece::setType(Julkinen::PalaTyyppi type){
	Type = type;
}
void Piece::setRotation(unsigned int rotation){
	Rotation = rotation;
}
void Piece::setLocation(Julkinen::Koordinaatti coord){
	Location = coord;
}
void Piece::setSpecialType(Julkinen::ErikoispalaTyyppi specialType){
	SpecialType = specialType;
}
void Piece::setTarget(Julkinen::Koordinaatti coord){
	Target = coord;
}
void Piece::setItem(char item){
	Item = item;
}
void Piece::removeItem(){
	Item = 'x';
}