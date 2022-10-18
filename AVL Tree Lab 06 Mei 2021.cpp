#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<malloc.h>

struct tree{
	int key;
	char name[50];
	tree *left, *right;
	int height;
}*root;

tree *createNode(int key, char *name){
	tree* temp = (tree*)malloc(sizeof(tree));
	temp->key=key;
	strcpy(temp->name,name);
	temp->left = temp->right= NULL;
	temp->height = 1; //inisial height dari node baru
	
	return temp;
}

int getHeight(tree* node){
	if(node == NULL){  //validasi juka current node == NULL , return 0
		return 0;
	}
	return node->height;
}

int max(int a, int b){
	//buat cari nilai maksimum a dari b
	return a>b?a:b;
}

//daapatkan kedalaman sebuah child node
//int getDepth(tree *node){
//	if(node==NULL){
//		return 0;
//	}
//	
//	int leftChild, rightChild;
//	leftChild  = getDepth(node->left);
//	rightChild = getDepth(node->right);
//	
//	//ternary
//	int result = leftChild >= rightChild ? leftChild : rightChild;
//	
//	return result + 1;
//}

int getBalanceFactor(tree *node){
	//return selisi kedalaman anak kanan dan kiri
	
	//>1 treenya berat dikiri dan wajib di rotate
	//<-1 treenya berat dikanan dan wajiib di rotate
	return getHeight(node->left) - getHeight(node->right);
}

tree *rotateRight(tree *y){
	tree *x = y->left;
	tree *t2 = x->right;
	
	x->right = y ;
	y->left = t2;
	
	//update height sih Y (karena posisi y dibawah x maka harus diupdate dulu)
	y->height = 1 + max(getHeight(y->left), getHeight(y->right));
	//update height sih X (karena posisinya diatas y , dan harus yang bawah dlu diupdate)
	x->height = 1 + max(getHeight(x->left),getHeight(x->right));
	
	return x;	
}

tree *rotateLeft(tree *x){
	tree *y = x->right;
	tree *t2 = y->left;
	
	y->left = x ;
	x->right = t2;
	
	//update height x dullu
	x->height = 1 + max(getHeight(x->left),getHeight(x->right));
	//update height y dulu
	y->height = 1 + max(getHeight(y->left), getHeight(y->right));
	
	return y;
}

tree *insert(tree *node, int key, char *name){
	//kalau nodenya kosong maka insert
	if(node==NULL){
		return createNode(key,name);
	}
	
	//insert seperti biasa
	if(node->key > key){
		node->left=insert(node->left,key,name);
	}
	else if(node->key < key){
		node->right = insert(node->right,key,name);
	}
	
	//Update heightnya setelah insert node yang baru
	node->height = 1 + max(getHeight(node->left),getHeight(node->right));
	
	//habis di insert, maka di hitung lgi balance factor
	//caranya adalah :
	//hitung jumlah anak yang paling dalam
	//diselisihkan antar anak kiri dan anak kanan
	
	int balance = getBalanceFactor(node);
	//[-1, 0 , 1] -> balance factor yang masih bisa ditoleransi
	if(balance > 1 && node->left->key > key){ //anak kiri lebih besar
		//left left case
		//maka harus rotate ke kanan 1 kali 
		return rotateRight(node);
	}
	else if(balance > 1 && node->left->key < key){
		//left right case
		//harus rotate kiri 1 kali baru ke kanan 1 kli 
		node->left = rotateLeft(node->left);
		return rotateRight(node);
	}
	else if(balance < -1 && node->right->key > key){
		//right left case
		//harus rotate ke kanan 1 baru ke kiri 1 
		node->right = rotateRight(node->right);
		return rotateLeft(node);
	}
	else if(balance < -1 && node->right->key < key){
		//right right case
		// rotate ke kiri
		return rotateLeft(node);
	}
	
	return node;
}

void printPreOrder(tree *node){
	if(node==NULL){
		return ;
	}
	
	printf("%d %s\n", node->key, node->name);
	printPreOrder(node->left);
	printPreOrder(node->right);
}

tree *search(tree* node, int key){
	if(node==NULL){
		return NULL;
	}
	
	if(node->key == key){
		return node;
	}
	else if(node->key > key){
		return search(node->left,key);
	}
	else if(node->key < key){
		return search(node->right,key);
	}
	
	return NULL;
}

tree* deleteNode(tree* node, int key){
	//base case
	if(node == NULL){
		puts("Data Not Found!!");
		return NULL;
	}
	
	if(node->key > key){
		//cari ke kiri
		node->left= deleteNode(node->left,key);
	}
	else if(node->key < key){
		node->right = deleteNode(node->right,key);
	}
	else{		
		//kondisi kalau ketemu dan mau didelete
		//Kondisi delte BST/AVL
		// 1. Kalau tidak punya ank, lgsg free current node
		//2. Kalau dia cuma punya 1 anak (kiri/kanan), replace current nodenya dengan anak tersebut(kiri/kanan)
		if(node->left == NULL || node->right == NULL){
			
			tree* successor = NULL;
			if(node->left){
				successor = node->left;
			}	
			else{
				//bisa jadi node->right adalah NULL / kondisi tidak punya ank sama sekali
				successor = node->right;
			}
			free(node);
			node = successor;
			if(node == NULL){
				return NULL;
			}		
		}
		else{
			//3. Kalau dia punya 2 anak, cari penerusnya dari anak kanan yang paling kiri
			//cari penerus dari anak kanan yang paling kiri
			tree* successor = node->right;
			while(successor->left){
				successor = successor->left;
			}
			node->key = successor->key;
			strcpy(node->name, successor->name);
			
			//delete successor yang ada dibawa karena nilainya sudah kita pindahin ke current node
			node->right = deleteNode(node->right,successor->key);
		
		}	
	}
	//update current height setelah melakukan update
	node->height = 1 + max(getHeight(node->left), getHeight(node->right));
	
	
	//rebalancing
	int balance_factor = getBalanceFactor(node);
	
	//lebih berat dikiri
	//left left	
	if(balance_factor > 1 && getBalanceFactor(node->left) >= 0){
		//singel rotate
		return rotateRight(node);
	}
	
	//lebih berat dikiri
	//left right
	if(balance_factor > 1 && getBalanceFactor(node->left) < 0){
		//double rotate
		node->left = rotateLeft(node->left);
		return rotateRight(node);
	}
	
	//lebih berat dikanan
	//right right
	if(balance_factor <-1 && getBalanceFactor(node->right) <= 0){
		//single rotate
		return rotateLeft(node);
	}
	
	//lebih berat dikanan
	//right left
	if(balance_factor <-1 && getBalanceFactor(node->right) > 0){
		//double rotate
		node->right = rotateRight(node->right);
		return rotateLeft(node);
	}
	return node;
}

tree* deleteALL(tree* node){
	//post order
	if(node==NULL){
		return NULL;
	}
	
	node->left=deleteALL(node->left);
	node->right=deleteALL(node->right);
	free(node);
	node=NULL;
	return node;
}

int main(){
	
	root = NULL;
	
	root = insert(root,30,"test1");
	root = insert(root,20,"test2");
	root = insert(root,50,"test3");
	root = insert(root,10,"test4");
	root = insert(root,40,"test5");
	root = insert(root,70,"test6");
	root = insert(root,35,"test7");
	printPreOrder(root);
	puts("==================");
	
	root = deleteNode(root,30);
	puts("=================");
	printPreOrder(root);
	
	root = deleteNode(root,20);
	puts("=================");
	printPreOrder(root);
	
	root = deleteNode(root,10);
	puts("=================");
	printPreOrder(root);
	
	puts("============");
	root = deleteALL(root);
	printPreOrder(root);
	puts("End");
	
//	printf("\nSetelah Update\n\n");
//	tree *temp = search(root, 10);
//	strcpy(temp->name, "asdasdasdas");
//	printPreOrder(root);
	
	
	getchar();
	return 0;
}












