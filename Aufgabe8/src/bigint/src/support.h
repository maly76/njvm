/*
 * support.h -- object representation and support functions
 */


#ifndef _SUPPORT_H_
#define _SUPPORT_H_

#define TRUE 1
#define FALSE 0

#define MSB (1 << (8 * sizeof(unsigned int) - 1))
#define IS_PRIMITIVE(objRef) (((objRef)->size & MSB) == 0)
#define GET_ELEMENT_COUNT(objRef) ((objRef)->size & ~MSB)

#define GET_REFS_PTR(objRef) ((ObjRef *) (objRef)->data)

#define MSBH (1 << (8 * sizeof(unsigned int) - 2))
#define HEART_BROKEN(objRef) (((objRef)->size & ~MSBH) == 0)

/* object representation */

typedef struct ObjRef {
    unsigned int size;
    void * forward_Pointer;
    unsigned char data[1];
} *ObjRef;

typedef struct {
    bool isDefined; //Versteh den Punkt nicht zu 100%
    bool isObjRef;
    union {
        ObjRef objRef; // isObjRef = TRUE
        int number; // isObjRef = FALSE
    } u;
} StackSlot;

typedef struct {
  ObjRef op1;			/* first (or single) operand */
  ObjRef op2;			/* second operand (if present) */
  ObjRef res;			/* result of operation */
  ObjRef rem;			/* remainder in case of division */
} BIP;


/* support functions */

void fatalError(char *msg);		/* print a message and exit */
ObjRef newPrimObject(int dataSize);	/* create a new primitive object */
bool isRootObject(ObjRef object);    /*check if an object is rootobject*/
void getAllRootObjects();

extern StackSlot *stack;
extern ObjRef *SDA;
extern ObjRef RVR;
extern bool statistic;
extern bool purge;
extern BIP bip;
extern int sp;
extern unsigned int info[4];


#endif /* _SUPPORT_H_ */
