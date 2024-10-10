#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct node {
	void *info;
	int address;
	struct node *next;
	struct node *prev;
} nod;

typedef struct lista {
	int data_size;
	int size;
	nod *head;
	nod *tail;
} list;

typedef struct vector {
	list *lista;
	long long val_start;
	int size;
	int tip_free;
	int bytes;
} free_list;

typedef struct vector2 {
	struct vector2 *array;
	int adresa;
	int size;
	void *data;
	int data_is_allocated;
	int data_len;
	int data_address;
} allocated_list;

typedef struct info {
	int alocated_memory;
	int free_memory;
	int free_blocks;
	int malloc_calls;
	int fragmentations;
	int nr_free_calls;
	int nr_aloc_blocks;
} info;

long long hexa_decimal(char *sir)
{
	int p16 = 1;
	long long nr = 0;
	for (int i = strlen(sir) - 1; i > 1; i--) {
		if (sir[i] == '0')
			nr += 0 * p16;
		if (sir[i] == '1')
			nr += 1 * p16;
		if (sir[i] == '2')
			nr += 2 * p16;
		if (sir[i] == '3')
			nr += 3 * p16;
		if (sir[i] == '4')
			nr += 4 * p16;
		if (sir[i] == '5')
			nr += 5 * p16;
		if (sir[i] == '6')
			nr += 6 * p16;
		if (sir[i] == '7')
			nr += 7 * p16;
		if (sir[i] == '8')
			nr += 8 * p16;
		if (sir[i] == '9')
			nr += 9 * p16;
		if (sir[i] == 'a')
			nr += 10 * p16;
		if (sir[i] == 'b')
			nr += 11 * p16;
		if (sir[i] == 'c')
			nr += 12 * p16;
		if (sir[i] == 'd')
			nr += 13 * p16;
		if (sir[i] == 'e')
			nr += 14 * p16;
		if (sir[i] == 'f')
			nr += 15 * p16;
		p16 *= 16;
	}
	return nr;
}

void add_node(list *lista, int address) {
	nod *new = malloc(sizeof(nod));
	new->address = address;
	new->info = malloc(lista->data_size);

	if (lista->size == 0) {
		new->next = NULL;
		new->prev = NULL;
		lista->head = new;
		lista->tail = new;
		lista->size++;
		return;
	} else {
		new->next = NULL;
		new->prev = lista->tail;
		lista->tail->next = new;
		lista->tail = new;
		lista->size++;
		return;
	}
}

free_list *init(char s[603], info *det) {
	char copie[603], *p;
	strcpy(copie, s);
	p = strtok(copie, " ");
	int nr_list, nr_bytes, tip_rec;
	long long ad_start;
	p = strtok(NULL, "\n ");
	ad_start = hexa_decimal(p);
	p = strtok(NULL, "\n ");
	sscanf(p, "%d", &nr_list);
	p = strtok(NULL, "\n ");
	sscanf(p, "%d", &nr_bytes);
	p = strtok(NULL, "\n ");
	if (p != NULL) {
		sscanf(p, "%d", &tip_rec);
	}

	int *nr_noduri = malloc(nr_list * sizeof(int));
	int b = 8;
	for (int i = 0; i < nr_list; i++) {
		nr_noduri[i] = nr_bytes / b;
		b *= 2;
	}

	free_list *c;
	c = (free_list *)malloc(sizeof(free_list));
	c->val_start = ad_start;
	c->size = nr_list;
	c->tip_free = tip_rec;
	c->bytes = nr_bytes;
	c->lista = malloc(nr_list * sizeof(list));

	b = 8;
	int x;
	for (int i = 0; i < nr_list; i++) {
		x =  ad_start + i * nr_bytes;
		c->lista[i].head = NULL;
		c->lista[i].tail = NULL;
		c->lista[i].size = 0;
		c->lista[i].data_size = b;
		for (int j = 0; j < nr_noduri[i]; j++) {
			int adresa = x + j * b;
			add_node(&c->lista[i], adresa);
			det->free_blocks++;
		}
		b *= 2;
	}

	free(nr_noduri);
	return c;
}

void my_malloc(free_list **fl, allocated_list **ol, char s[603], info *det) {
	char *p = strtok(s, " \n");
	p = strtok(NULL, " \n");
	int bytes;
	sscanf(p, "%d", &bytes);
	int ok = 0;

	for (int i = 0; i < (*fl)->size; i++) {
		if (bytes == (*fl)->lista[i].data_size && (*fl)->lista[i].size > 0) {
			nod *scos = (*fl)->lista[i].head;
			if ((*fl)->lista[i].size == 1) {
				(*fl)->lista[i].head = NULL;
				(*fl)->lista[i].tail = NULL;
			} else if ((*fl)->lista[i].size > 1) {
				(*fl)->lista[i].head = (*fl)->lista[i].head->next;
				(*fl)->lista[i].head->prev = NULL;
			}
			(*fl)->lista[i].size--;
			(*ol)->size++;
			(*ol)->array = realloc((*ol)->array, (*ol)->size * sizeof(struct vector2));
			(*ol)->array[(*ol)->size - 1].adresa = scos->address;
			(*ol)->array[(*ol)->size - 1].size = (*fl)->lista[i].data_size;
			(*ol)->array[(*ol)->size - 1].data_is_allocated = 0;
			(*ol)->array[(*ol)->size - 1].data_len = 0;
			free(scos->info);
			free(scos);
			ok = 1;
		} else if (bytes < (*fl)->lista[i].data_size  && (*fl)->lista[i].size > 0) {
			det->fragmentations++;
			det->free_blocks++;
			int rest = (*fl)->lista[i].data_size - bytes;
			int exista = 0;
			int poz;
			for (int j = 0; j < (*fl)->size && exista == 0; j++)
				if ((*fl)->lista[j].data_size == rest) {
					exista = 1;
					poz = j;
				}

			nod *scos2 = (*fl)->lista[i].head;
			if ((*fl)->lista[i].size == 1) {
				(*fl)->lista[i].head = NULL;
				(*fl)->lista[i].tail = NULL;
			} else if ((*fl)->lista[i].size > 1) {
				(*fl)->lista[i].head = (*fl)->lista[i].head->next;
				(*fl)->lista[i].head->prev = NULL;
			}
			(*fl)->lista[i].size--;
			if (exista == 1) {
				nod *nou = (*fl)->lista[poz].head;
				nod *new = malloc(sizeof(*new));
				new->address = scos2->address + bytes;
				new->info = malloc((*fl)->lista[poz].data_size);

				if (nou) {
					while (nou && nou->address < new->address)
						nou = nou->next;

					if (nou == (*fl)->lista[poz].head) {
						new->next = nou;
						new->prev = NULL;
						(*fl)->lista[poz].head->prev = new;
						(*fl)->lista[poz].head = new;
					} else if (nou) {
						new->next = nou;
						new->prev = nou->prev;
						nou->prev->next = new;
						nou->prev = new;
					} else if (!nou) {
						add_node(&(*fl)->lista[poz], scos2->address + bytes);
						(*fl)->lista[poz].size--;
						free(new->info);
						free(new);
					}
				} else {
					new->next = NULL;
					new->prev = NULL;
					(*fl)->lista[poz].head = new;
					(*fl)->lista[poz].tail = new;
				}
				(*fl)->lista[poz].size++;
			} else {
				(*fl)->size++;
				(*fl)->lista = realloc((*fl)->lista, (*fl)->size * sizeof(list));
				int okk = 0, poz2 = -1;
				for (int j = 0; j < (*fl)->size && okk == 0; j++) {
					if ((*fl)->lista[j].data_size > rest) {
						okk = 1;
						poz2 = j;
					}
				}
				if (poz2 == -1)
					poz2 = (*fl)->size - 1;

				for (int k = (*fl)->size - 1; k > poz2; k--) {
					(*fl)->lista[k] = (*fl)->lista[k - 1];
				}

				(*fl)->lista[poz2].head = NULL;
				(*fl)->lista[poz2].tail = NULL;
				(*fl)->lista[poz2].size = 0;
				(*fl)->lista[poz2].data_size = rest;
				add_node(&(*fl)->lista[poz2], scos2->address + bytes);
			}

			(*ol)->size++;
			(*ol)->array = realloc((*ol)->array, (*ol)->size * sizeof(struct vector2));
			(*ol)->array[(*ol)->size - 1].adresa = scos2->address;
			(*ol)->array[(*ol)->size - 1].size = bytes;
			(*ol)->array[(*ol)->size - 1].data_is_allocated = 0;
			(*ol)->array[(*ol)->size - 1].data_len = 0;
			free(scos2->info);
			free(scos2);
			ok = 1;
		}
		if (ok == 1) {
			det->alocated_memory += bytes;
			det->nr_aloc_blocks++;
			det->free_blocks--;
			det->free_memory -= bytes;
			det->malloc_calls++;

			allocated_list *n = malloc(sizeof(struct vector2));
			n->array = malloc(sizeof(struct vector2));

			for (int i = 0; i < (*ol)->size - 1; i++)
				for (int j = i + 1; j < (*ol)->size; j++) {
					if ((*ol)->array[i].adresa > (*ol)->array[j].adresa) {
						n->array[0] = (*ol)->array[i];
						(*ol)->array[i] = (*ol)->array[j];
						(*ol)->array[j] = n->array[0];
					}
				}
			free(n->array);
			free(n);
			return;
		}
	}

	if (ok == 0) {
		printf("Out of memory\n");
	}

}

void eliberare(free_list **fl, allocated_list **ol, char s[603], info *det) {
	int adresa_eliberare;
	char *p;
	p = strtok(s, " \n");
	p = strtok(NULL, " \n");
	adresa_eliberare = hexa_decimal(p);

	int ok = 0, ok2 = 0;
	int poz;

	for (int i = 0; i < (*ol)->size && ok == 0; i++) {
		if ((*ol)->array[i].adresa == adresa_eliberare) {
			ok = 1;
			poz = i;
			det->free_memory += (*ol)->array[poz].size;
			det->nr_free_calls++;
		}
	}

	if (ok == 1) {
		det->free_blocks++;
		det->alocated_memory -= (*ol)->array[poz].size;
		det->nr_aloc_blocks--;
		int gata = 0;
		for (int i = 0; i < (*fl)->size && gata == 0; i++) {
			if ((*fl)->lista[i].data_size == (*ol)->array[poz].size) {
				nod *new = malloc(sizeof(*new));
				new->address = (*ol)->array[poz].adresa;
				new->info = malloc((*fl)->lista[i].data_size);
				nod *n = (*fl)->lista[i].head;

				if (n) {
					while (n && n->address < new->address)
						n = n->next;

					if (n == (*fl)->lista[i].head) {
						new->prev = NULL;
						new->next = n;
						(*fl)->lista[i].head->prev = new;
						(*fl)->lista[i].head = new;
					}
					else if (n) {
						new->next = n;
						new->prev = n->prev;
						n->prev->next = new;
						n->prev = new;
					}
					else if (!n) {
						add_node(&(*fl)->lista[i], (*ol)->array[poz].adresa);
						(*fl)->lista[i].size--;
						free(new->info);
						free(new);
					}
				} else {
					new->next = NULL;
					new->prev = NULL;
					(*fl)->lista[i].head = new;
					(*fl)->lista[i].tail = new;
				}
				(*fl)->lista[i].size++;
				gata = 1;
			}
		}

		if (gata == 0) {
			int adr = (*ol)->array[poz].adresa;
			int dim = (*ol)->array[poz].size;
			int terminat = 0;
			int index;
			for (int i = 0; i < (*fl)->size && terminat == 0; i++) {
				if ((*fl)->lista[i].data_size > dim) {
					terminat = 1;
					index = i;
				}
			}

			(*fl)->size++;
			(*fl)->lista = realloc((*fl)->lista, (*fl)->size * sizeof(list));
			for (int i = (*fl)->size - 1; i > index; i--)
				(*fl)->lista[i] = (*fl)->lista[i - 1];
			
			(*fl)->lista[index].head = NULL;
			(*fl)->lista[index].tail = NULL;
			(*fl)->lista[index].size = 0;
			(*fl)->lista[index].data_size = (*ol)->array[poz].size;
			add_node(&(*fl)->lista[index], adr);
		}
		if ((*ol)->array[poz].data_is_allocated == 1) {
			free((*ol)->array[poz].data);
			(*ol)->array[poz].data_is_allocated = 0;
		}
		for (int i = poz; i < (*ol)->size - 1; i++)
			(*ol)->array[i] = (*ol)->array[i + 1];
		(*ol)->size--;
		(*ol)->array = realloc((*ol)->array, (*ol)->size * sizeof(struct vector2));
	} else {
		printf("Invalid free\n");
		return;
	}
}

void dump_memory(free_list *lista, allocated_list *ol,info det, int memorie) {
	printf("+++++DUMP+++++\n");

	printf("Total memory: %d bytes\n", memorie);
	printf("Total allocated memory: %d bytes\n", det.alocated_memory);
	printf("Total free memory: %d bytes\n", det.free_memory);
	printf("Free blocks: %d\n", det.free_blocks);
	printf("Number of allocated blocks: %d\n", det.nr_aloc_blocks);
	printf("Number of malloc calls: %d\n", det.malloc_calls);
	printf("Number of fragmentations: %d\n", det.fragmentations);
	printf("Number of free calls: %d\n", det.nr_free_calls);

	for (int i = 0; i < lista->size; i++) {
		if (lista->lista[i].size != 0) {
			printf("Blocks with %d bytes - %d free block(s) : ", lista->lista[i].data_size, lista->lista[i].size);
			nod *n = lista->lista[i].head;
			while (n->next != NULL) {
				printf("0x%x ", n->address);
				n = n->next;
			}
			printf("0x%x", n->address);
			printf("\n");
		}
	}

	printf("Allocated blocks :");
	if (ol->size != 0) {
		int i;
		for (i = 0; i < ol->size; i++)
			printf(" (0x%x - %d)", ol->array[i].adresa, ol->array[i].size);
	}
	printf("\n");

	printf("-----DUMP-----\n");
}

void free_list1(nod *node) {
	 while (node != NULL) {
		nod *aux = node;
		node = node->next;
		free(aux->info);
		free(aux);
	}
}

void dump_heap(free_list *lista) {
	for (int i = 0; i < lista->size; i++) {
		free_list1(lista->lista[i].head);
	}
	free(lista->lista);
	free(lista);
}

void write(free_list *lista, allocated_list **ol, char sir[603], int memorie, info det) {
	char *p, sir2[603];
	strcpy(sir2, sir);
	int adr, bytes;
	p = strtok(sir, " \n");
	p = strtok(NULL, " \n");
	adr = hexa_decimal(p);
	char s[strlen(sir2)], c[strlen(sir2)];
	int k = 0, l = 0;
	int cnt = 0, spatiu;
	for (int i = 0; cnt != 2; i++)
		if (sir2[i] == ' ') {
			cnt++;
			spatiu = i + 1;
		}
	int reluam;
	int brk = 1;
	for (int i = spatiu + 1; brk == 1; i++) {
		if (sir2[i] == '"' && sir[i + 1] == ' ')
			brk = 0;
		else {
			s[k++] = sir2[i];
			reluam = i;
		}
	}
	s[k] = '\0';

	for (int i = reluam + 2; sir2[i] != '\0'; i++)
		c[l++] = sir2[i];
	c[l] = '\0';
	sscanf(c, "%d", &bytes);

	int interval = adr + bytes - 1;
	int ok = 0;
	int poz;
	int dim;
	int dif, rest;
	for (int i = 0; i < (*ol)->size && ok == 0; i++)
		if (adr >= (*ol)->array[i].adresa && adr <= (*ol)->array[i].adresa + (*ol)->array[i].size - 1) {
			ok = 1;
			poz = i;
			dim = (*ol)->array[i].size;
			dif = adr - (*ol)->array[i].adresa;
			rest = (*ol)->array[i].size - dif;
		}

	if (ok == 0) {
		printf("Segmentation fault (core dumped)\n");
		dump_memory(lista, (*ol), det,memorie);
		dump_heap(lista);
		for (int i = 0; i < (*ol)->size; i++)
			if ((*ol)->array[i].data_is_allocated == 1) {
				free((*ol)->array[i].data);
			}
		free((*ol)->array);
		free(*ol);
		exit(0);
	} else {
		if (bytes >= strlen(s))
			bytes = strlen(s);

		int exista_loc = 1;
		int nr_block = 1;
		int mem_continua = 1;
		int ultimul = poz;
		int total = rest;

		for (int i = poz + 1; i < (*ol)->size && total <= bytes && mem_continua == 1; i++) {
			if ((*ol)->array[i].size == dim && (*ol)->array[i].adresa == (*ol)->array[i - 1].adresa + dim) {
				ultimul = i;
				total += dim;
			}
			else {
				mem_continua = 0;
			}
		}

		if (total < bytes)
			exista_loc = 0;
		
		if (exista_loc == 0 || mem_continua == 0) {
			printf("Segmentation fault (core dumped)\n");
			dump_memory(lista, (*ol), det,memorie);
			dump_heap(lista);
			for (int i = 0; i < (*ol)->size; i++)
				if ((*ol)->array[i].data_is_allocated == 1) {
					free((*ol)->array[i].data);
				}
			free((*ol)->array);
			free(*ol);
			exit(0);
		} else if (exista_loc == 1 && mem_continua == 1) {
			int ap = 0;
			for (int i = poz; i <= ultimul; i++) {
				if ((*ol)->array[i].data_is_allocated == 0) {
					(*ol)->array[i].data = malloc(dim);
					if (bytes <= rest) {
						if (i == poz) {
							memcpy((*ol)->array[i].data + dif, s, bytes);
						} else {
							memcpy((*ol)->array[i].data, s + rest + dim * ap, bytes);
						}
					} else {
						if (i == poz) {
							memcpy((*ol)->array[i].data + dif, s, rest);
							(*ol)->array[i].data_len = rest;
							bytes -= rest;
						} else {
							if (bytes <= dim) {
								memcpy((*ol)->array[i].data, s + rest + dim * ap, bytes);
								ap++;
							} else {
								memcpy((*ol)->array[i].data, s + rest + dim * ap, dim);
								ap++;
							}
						}
					}
					(*ol)->array[i].data_is_allocated = 1; 
				} else {
					if (bytes <= rest) {
						if (i == poz) {
							memcpy((*ol)->array[i].data + dif, s, bytes);
						} else {
							memcpy((*ol)->array[i].data, s + dif + dim * ap, bytes);
						}
					} else {
						if (i == poz) {
							memcpy((*ol)->array[i].data + dif, s, rest);
							bytes -= rest;
						} else {
							if (bytes <= dim) {
								memcpy((*ol)->array[i].data, s + rest + dim * ap, bytes);
								ap++;
							} else {
								memcpy((*ol)->array[i].data, s + rest + dim * ap, dim);
								ap++;
							}
						}
					}
				}
			}
		}
	}
}

void my_read(allocated_list *ol, free_list *fl, int mem, info det, char sir[603]) {
	char *p;
	int adresa, bytes;
	p = strtok(sir, " \n");
	p = strtok(NULL, " \n");
	adresa = hexa_decimal(p);
	p = strtok(NULL , " \n");
	sscanf(p, "%d", &bytes);

	int ok = 0, poz, dim;
	int dif, rest;
	for (int i = 0; i < ol->size && ok == 0; i++)
		if (adresa >= ol->array[i].adresa && adresa <= ol->array[i].adresa + ol->array[i].size - 1) {
			ok = 1;
			poz = i;
			dim = ol->array[i].size;
			dif = adresa - ol->array[i].adresa;
			rest = ol->array[i].size - dif;
		}

	if (ok == 0) {
		printf("Segmentation fault (core dumped)\n");
		dump_memory(fl, ol, det, mem);
		dump_heap(fl);
			for (int i = 0; i < ol->size; i++)
				if (ol->array[i].data_is_allocated == 1) {
					free(ol->array[i].data);
				}
			free(ol->array);
			free(ol);
			exit(0);
	} else {
		int e_continua = 1;
		int e_loc = 1;
		int total = dim;
		int ultimul = poz;
		for (int i = poz + 1; i < ol->size && total < bytes && e_continua == 1; i++) {
			if (ol->array[i].size == dim && ol->array[i].adresa == ol->array[i - 1].adresa + dim) {
				ultimul = i;
				total += dim;
			}
			else e_continua = 0;
		}

		if (total < bytes)
			e_loc = 0;

		if (e_loc == 0 || e_continua == 0){
			printf("Segmentation fault (core dumped)\n");
			dump_memory(fl, ol, det, mem);
			dump_heap(fl);
			for (int i = 0; i < ol->size; i++)
				if (ol->array[i].data_is_allocated == 1) {
					free(ol->array[i].data);
				}
			free(ol->array);
			free(ol);
			exit(0);
		} else {
			int cnt = 0;
			for (int i = poz; i <= ultimul; i++) {
				if (i == poz) {
					int k = 0;
					int p = 0;
					while (p < bytes && cnt != bytes && k == 0) {
						if (p == ol->array[i].size - dif)
							k = 1;
						else {
							printf("%c", ((char*)ol->array[i].data + dif)[p]);
							p++;
							cnt++;
						}
					}
				} else {
					int kk = 0;
					int p = 0;
					while (p < bytes && cnt != bytes && kk == 0) {
						if (p == ol->array[i].size)
							kk = 1;
						else {
							printf("%c", ((char*)ol->array[i].data)[p]);
							p++;
							cnt++;
						}
					}
				}
			}
			printf("\n");
		}
	}
}

int main(void) {
	char sir[603], cop_sir[603], *p;
	int ok = 1;
	free_list* lista_libera;
	allocated_list* lista_ocupata;
	lista_ocupata = malloc(sizeof(*lista_ocupata));
	lista_ocupata->size = 0;
	lista_ocupata->array = malloc(sizeof(allocated_list));
	info detalii;
	detalii.alocated_memory = 0;
	detalii.fragmentations = 0;
	detalii.free_blocks = 0;
	detalii.free_memory = 0;
	detalii.malloc_calls = 0;
	detalii.nr_free_calls = 0;
	detalii.nr_aloc_blocks = 0;
	int mem;
	while (ok == 1) {
		fgets(sir, 603, stdin);
		strcpy(cop_sir, sir);
		p = strtok(cop_sir, " \n");
		if (strcmp(p, "INIT_HEAP") == 0) {
			lista_libera = init(sir, &detalii);
			mem = lista_libera->size * lista_libera->bytes;
			detalii.free_memory = mem;
		}
		if (strcmp(p, "MALLOC") == 0) {
			my_malloc(&lista_libera, &lista_ocupata, sir, &detalii);
		}
		if (strcmp(p, "FREE") == 0) {
			eliberare(&lista_libera, &lista_ocupata, sir, &detalii);
		}
		if (strcmp(p, "READ") == 0) {
			my_read(lista_ocupata, lista_libera, mem, detalii, sir);
		}
		if (strcmp(p, "WRITE") == 0) {
			write(lista_libera, &lista_ocupata, sir, mem, detalii);
		}
		if (strcmp(p, "DUMP_MEMORY") == 0) {
			dump_memory(lista_libera, lista_ocupata, detalii, mem);
		}
		if (strcmp(p, "DESTROY_HEAP") == 0) {
			dump_heap(lista_libera);
			for(int i = 0; i < lista_ocupata->size; i++)
				if(lista_ocupata->array[i].data_is_allocated == 1){
					free(lista_ocupata->array[i].data);
				}
			free(lista_ocupata->array);
			free(lista_ocupata);
			ok = 0;
		}
	}
	return 0;
}
