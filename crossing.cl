#define N_PARENT $$N_PARENT                                                                                                                                                        
#define N_CHROMOSECTOR_IN_CHROMOSET $$N_CHROMOSECTOR_IN_CHROMOSET
#define N_CHROMOSET_IN_LOC $$N_CHROMOSET_IN_LOC                                                                                                                                                        
#define N_CHROMOSET_IN_GLOB $$N_CHROMOSET_IN_GLOB
#define START_RATING $$START_RATING                                                                                                                                                    
#define N_GLOBAL_CICLES $$N_GLOBAL_CICLES				// Количество итераций глобального цикла
#define N_LOCAL_CICLES $$N_LOCAL_CICLES					// Количество итераций локального цикла
#define K_RATING $$K_RATING						// Коэффициент рейтинга
#define KOEF_A $$KOEF_A                                                                                                                                                        
#define KOEF_C $$KOEF_C    

typedef uint ChromoSector; 
typedef uint Index; 
typedef ChromoSector ChromoSet[N_CHROMOSECTOR_IN_CHROMOSET]; 

// Копирование одного набора хромосом.                                                                                                                        
inline void copy_ChromoSet_to_Loc(__local ChromoSet* dst_ChromoSet, __global ChromoSet* src_ChromoSet)                                                                                                                                                                                         
{                                                                                                                                                                     
    if (get_local_id(0) == 0)
    {	
        for (int i=0; i<N_CHROMOSECTOR_IN_CHROMOSET; ++i)
	{
            (*dst_ChromoSet)[i] = (*src_ChromoSet)[i];      
	}
    }
}                                                                                                                                                                     


typedef struct                                                                                                                                                         
{                                                                                                                                                                        
    ChromoSet 	chromoSets[N_CHROMOSET_IN_LOC];                                                                                                                                                
    Index 	glob_indexes[N_CHROMOSET_IN_LOC];                                                                                                                                                    
    ChromoSet 	sampleChromoSet;                                                // Указатель на образцовую особь                                                                                    
    ChromoSet 	invertSampleChromoSet;                                          // Указатель на инвертированную образцовую особь                                                                    
}  Loc; //__attribute__ ((packed))                                                                                                                                   



__kernel void crossing         (                                                                                                                                            
          // Входа                                                                                                                                                        
          __global ChromoSet* chromoSets,                                     // Указатель на приведенный одномерный массив входных советников                                                    
          __global ChromoSet* sampleChromoSet,                                // Указатель на образцовую особь                                                                                    
          __global ChromoSet* invertSampleChromoSet,                          // Указатель на инвертированную образцовую особь                                                                    
                                                                                                                                                                        
          // Локальные                                                                                                                                                    
          __local Loc* loc,                                                     // Указатель на приведенный одномерный массив входных советников в локальной памяти                                     
                                                                                                                                                                        
          // Выхода                                                                                                                                                        
          __global Index* res_glob_indexes,                                     // Указатель на массив выходных советников                                                                            
          __global int* res_ratings,                                     		// Указатель на массив выходных советников                                                                            
          __global int* res_c11s,                                     		// Указатель на массив выходных советников                                                                            
          __global int* res_c01s                                     		// Указатель на массив выходных советников                                                                            
          )                                                                                                                                                                
{   
    res_ratings[get_global_id(0)] = 0;
    res_c11s[get_global_id(0)] = 0;
    res_c01s[get_global_id(0)] = 0;
    res_glob_indexes[get_global_id(0) * N_PARENT + $$EXTRACT_N_PARENT] = 0;         
	
    copy_ChromoSet_to_Loc(&(*loc).sampleChromoSet, sampleChromoSet);
    copy_ChromoSet_to_Loc(&(*loc).invertSampleChromoSet, invertSampleChromoSet);    
    
    barrier( CLK_LOCAL_MEM_FENCE );
  
    int best_rating = START_RATING;

    Index index = get_global_id(0);      

    for (int n = 0; n < N_GLOBAL_CICLES; ++n)                                   // Глобальный цикл                                                                                                        
    {                                                                                                                                                                    

        for (int i = 0; i < N_CHROMOSET_IN_LOC; ++i)                                                                                                                          
        {                                                                                                                                                                 
            index += (int)mad(KOEF_C, (float)get_local_id(0), KOEF_A) ^ get_global_id(0);
	    index += get_global_id(0) << 8;	

            Index glob_index = index % N_CHROMOSET_IN_GLOB;
  
            if (get_local_id(0) == 0) 
            {
                copy_ChromoSet_to_Loc(&(*loc).chromoSets[i], &chromoSets[glob_index]);                                                                     
	        (*loc).glob_indexes[i] = glob_index; 
	    }
 	}
        

        for (int m = 0; m < N_LOCAL_CICLES; ++m)                                // Локальный цикл.                                                                                                    
        {                                                                                                                                                                
    	    barrier( CLK_LOCAL_MEM_FENCE );

            index += (int)mad(KOEF_A, (float)index, KOEF_C) ^ get_global_id(0); Index iChromoSet$$EXTRACT_N_PARENT = index % N_CHROMOSET_IN_LOC;

            int c11 = 0;                                                                                                            
            int c01 = 0;                                                                                                        

            for (int i=0; i<N_CHROMOSECTOR_IN_CHROMOSET; ++i)                                                                                                                                    
            {                                                                                                                                                            

                ChromoSector x$$EXTRACT_N_PARENT = (*loc).chromoSets[iChromoSet$$EXTRACT_N_PARENT][i];                                                                                                                                      

                                                                                                                                                                      
                ChromoSector resChromoSector = $$EXPRESSION;
                
                c11 += popcount((*loc).sampleChromoSet[i] & resChromoSector);                                                                                                            

                c01 += popcount((*loc).invertSampleChromoSet[i] & resChromoSector);                                                                                                        


            }                                                                                                                                                             


            int rating = mad(c11, K_RATING, -c01);         

		
            if (rating > best_rating)                                                                                                                                    
            {   
                int gid = get_global_id(0);                  
                res_ratings[gid] = rating;
                res_c11s[gid] = c11;
                res_c01s[gid] = c01;
                res_glob_indexes[gid * N_PARENT + $$EXTRACT_N_PARENT] = (*loc).glob_indexes[iChromoSet$$EXTRACT_N_PARENT];         


		best_rating = rating;	                                  
            }                 
        }                                                                                                                                                                 
    }      
}                                                                                                                                                                        


