//
// Created by DONTO on 4/25/2019.
//

#include <stdio.h>
#include "eurovision.h"
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#define NUMBER_OF_RESULTS 10

char* stringCopy(const char* str) {
    if(str == NULL){
        return NULL;
    }
    int strLen =(int) strlen(str);
    char *strDest = malloc(sizeof(strLen + 1));
    if (strDest == NULL) {
        return NULL;
    } else {
        strcpy(strDest, str);
    }
    return strDest;
}


static StateDataMap copyStateData(StateDataMap dataToCopy){
    if(dataToCopy == NULL){
        return NULL;
    }
    StateData data =(StateData) dataToCopy;
    StateData stateDataNew = malloc(sizeof(*stateDataNew));
    if(stateDataNew == NULL){
        return NULL;
    } else{
        stateDataNew->stateName = stringCopy(data->stateName);
        if(stateDataNew->stateName == NULL) {
            return NULL;
        }
        stateDataNew->songName = stringCopy(data->songName);
        if(stateDataNew->songName == NULL){
            free(data->stateName);
            return NULL;
        }
        Map citizenVoteDest = mapCopy(data->citizenVote);
        if(citizenVoteDest == NULL){
            free(data->stateName);
            free(data->songName);
            return NULL;
        }
        data->citizenVote = citizenVoteDest;
    }
    return stateDataNew;
}
/** Function to be used by the map for freeing elements */
static void freeStateData(StateDataMap dataToFree) {
    StateData toFree= (StateData) dataToFree;
    free(toFree->stateName);
    free(toFree->songName);
    mapDestroy(toFree->citizenVote);
}


static VoteDataElement copyVoteDataElement(VoteDataElement voteToCopy){
    if(voteToCopy == NULL){
        return NULL;
    }
    int *ptr = malloc(sizeof(int));
    if(ptr == NULL){
        return NULL;
    }
    *ptr = *(int*)voteToCopy;
    return ptr;
}

static KeyElement copyKeyElement(KeyElement keyToCopy){
    if(keyToCopy == NULL){
        return NULL;
    }
    int *ptr = malloc(sizeof(int));
    if(ptr == NULL){
        return NULL;
    }
    *ptr = *(int*)keyToCopy;
    return ptr;
}

void freeVoteDataElement(VoteDataElement voteToFree){
    free(voteToFree);
}

void freeKeyElement(KeyElement keyToFree){
    free(keyToFree);
}

int compareKeyElements(KeyElement key1,KeyElement key2){
    int a = *(int*)key1;
    int b = *(int*)key2;
    if (a > b){
        return 1;
    }
    else if (a == b) {
        return 0;

    } else { //the second is bigger
        return -1;
    }

    static  JudgeDataMap copyJudgeDataElement(JudgeDataMap judgeDataToCopy) {
        if(judgeDataToCopy == NULL){
            return NULL;
        }
        JudgeData data =(JudgeData) judgeDataToCopy;
        JudgeData judgeDataNew = malloc(sizeof(*judgeDataNew));
        if(judgeDataNew == NULL){
            return NULL;
        }else {
            judgeDataNew->judgeName = stringCopy(data->judgeName);
            if (judgeDataNew->judgeName == NULL) {
                freeJudgeDataElement(judgeDataNew);
                return NULL;
            }
            judgeDataNew->judgeResults = malloc((sizeof(int))*NUMBER_OF_RESULTS);
            if(judgeDataNew->judgeResults == NULL){
                freeJudgeDataElement(judgeDataNew);
                free(judgeDataNew->judgeName);
                return NULL;
            }
            for (int i = 0; i < NUMBER_OF_RESULTS; i++) {
                judgeDataNew->judgeResults[i] = judgeDataToCopy->judgeResults[i]; ///is it ok?
            }
            return judgeDataNew;
        }
    }

    static JudgeDataMap freeJudgeDataElement(JudgeDataMap judgeDataToFree){
        JudgeData ptr = (JudgeData) judgeDataToFree;
        free(ptr->judgeName);
        free(ptr->judgeResults);
    }

    Eurovision eurovisionCreate(){
        Eurovision eurovision = malloc(sizeof(*eurovision));
        if(eurovision == NULL){
            return NULL;
        }
        eurovision->state = mapCreate(copyStateData,copyKeyElement,freeStateData,freeKeyElement,compareKeyElements);
        if(eurovision->state == NULL){
            free(eurovision);
            return NULL;
        }
        eurovision->judge = mapCreate(copyJudgeDataElement,copyKeyElement,freeJudgeDataElement,freeKeyElement,
                                      compareKeyElements);
        if(eurovision->judge == NULL){
            free(eurovision);
            free(eurovision->state);
            return NULL;
        }
        return eurovision;
    }

    void eurovisionDestroy(Eurovision eurovision){
        mapDestroy(eurovision->state);
        mapDestroy(eurovision->judge);
    }
    int main()
    {
        Eurovision h= eurovisionCreate();

    }
    bool checkIfNotNegitive(int num){
        if(num < 0) {
            return false;
        } else{
            return true;
        }
    }

    bool checkIfNameIsLegal(const char *name){
        char *copyName = stringCopy(name);
        while (*copyName != '\0'){
            if(((*copyName <= 'a') || (*copyName >= 'z')) && *copyName != ' '){
                free(copyName);
                return false;
            }
        }
        free(copyName);
        return true;
    }
    EurovisionResult eurovisionAddState(Eurovision eurovision, int stateId, const char *stateName, const char *songName)
    {
        if(!checkIfNotNegitive(stateId)){
            return EUROVISION_INVALID_ID;
        } else if(mapContains(eurovision->state, &stateId)){
            return EUROVISION_STATE_ALREADY_EXIST;
        }else if( !checkIfNameIsLegal(stateName) || !checkIfNameIsLegal(songName)){
            return EUROVISION_INVALID_NAME;
        }else{
            StateData  newStateData =  (StateData) malloc(sizeof(*newStateData));
            if(newStateData == NULL){
                eurovisionDestroy(eurovision);
                return EUROVISION_OUT_OF_MEMORY;
            }
            newStateData->stateName = stringCopy(stateName);
            if(newStateData->stateName == NULL){
                freeStateData(newStateData);
                free(newStateData);
                eurovisionDestroy(eurovision);
                return EUROVISION_OUT_OF_MEMORY;
            }
            newStateData->songName = stringCopy(songName);
            if(newStateData->songName == NULL){
                freeStateData(newStateData);
                free(newStateData);
                eurovisionDestroy(eurovision);
                return EUROVISION_OUT_OF_MEMORY;
            }
            MapResult result = mapPut(eurovision->state,&stateId,&newStateData);
            if(result == MAP_OUT_OF_MEMORY) {
                freeStateData(newStateData);
                free(newStateData);
                eurovisionDestroy(eurovision);
                return EUROVISION_OUT_OF_MEMORY;
            }else{
                return EUROVISION_SUCCESS;
            }
        }
    }

    EurovisionResult eurovisionRemoveState(Eurovision eurovision, int stateId){
        if(!checkIfNotNegitive(stateId)){
            return EUROVISION_INVALID_ID;
        }else{
            MapResult result = mapRemove(eurovision->state,&stateId);
            if(result == MAP_ITEM_DOES_NOT_EXIST){
                return EUROVISION_STATE_NOT_EXIST;
            }
            MAP_FOREACH(int *,iter,eurovision->state)
            {
                StateData stateData = mapGet(eurovision->state,iter);
                if(stateData == NULL){
                    return EUROVISION_NULL_ARGUMENT;
                }
                Map citizenVote = stateData->citizenVote;
                MapResult result1 = mapRemove(citizenVote, &stateId);
                if(result1 == MAP_NULL_ARGUMENT){
                    return EUROVISION_NULL_ARGUMENT;
                }
            }
            MapResult result2 = mapRemove(eurovision->state, &stateId);
            if(result2 == MAP_NULL_ARGUMENT){
                return EUROVISION_NULL_ARGUMENT;
            }
        }
    }

    EurovisionResult eurovisionAddJudge(Eurovision eurovision, int judgeId,const char *judgeName,int *judgeResults){
        if((eurovision == NULL) || (judgeName == NULL) || (judgeResults == NULL)){
            return EUROVISION_NULL_ARGUMENT;
        }
        if(!checkIfNotNegitive(judgeId)){
            return EUROVISION_INVALID_ID;
        } else if(mapContains(eurovision->judge, &judgeId)){
            return EUROVISION_JUDGE_ALREADY_EXIST;
        }else if( !checkIfNameIsLegal(judgeName)) {
            return EUROVISION_INVALID_NAME;
        }
        JudgeData  newJudgeData =  (JudgeData) malloc(sizeof(*newJudgeData));
        if(newJudgeData == NULL){
            eurovisionDestroy(eurovision);
            return EUROVISION_OUT_OF_MEMORY;
        }
        newJudgeData->judgeName = stringCopy(judgeName);
        if(newJudgeData->judgeName == NULL){
            free(newJudgeData);
            eurovisionDestroy(eurovision);
            return EUROVISION_OUT_OF_MEMORY;
        }
        newJudgeData->judgeResults = malloc(sizeof(int)*NUMBER_OF_RESULTS);
        if(newJudgeData->judgeResults == NULL){
            free(newJudgeData->judgeName);
            free(newJudgeData);
            return EUROVISION_OUT_OF_MEMORY;
        }
        for (int i = 0; i < NUMBER_OF_RESULTS; i++) {
            newJudgeData->judgeResults[i] = newJudgeData->judgeResults[i]; ///is it ok?
        }
        MapResult result = mapPut(eurovision->judge,&judgeId,&newJudgeData); ///&?
        if(result == MAP_OUT_OF_MEMORY) {
            freeJudgeDataElement(newJudgeData);
            free(newJudgeData);
            eurovisionDestroy(eurovision);
            return EUROVISION_OUT_OF_MEMORY;
        }else{
            return EUROVISION_SUCCESS;
        }



        EurovisionResult eurovisionRemoveJudge(Eurovision eurovision, int judgeId);

        EurovisionResult eurovisionAddVote(Eurovision eurovision, int stateGiver, int stateTaker)
        {
            if ((!checkIfNotNegitive(stateGiver)) || (!checkIfNotNegitive(stateTaker))) {
                return EUROVISION_INVALID_ID;
            } else if (stateGiver == stateTaker) {
                return EUROVISION_SAME_STATE;
            } else {
                bool result1 = mapContains(eurovision->state, &stateGiver);
                bool result2 = mapContains(eurovision->state, &stateTaker);
                if ((!result1) || (!result2)) {
                    return EUROVISION_STATE_NOT_EXIST;
                }
            }
            StateData stateData = (StateData) mapGet(eurovision->state, &stateGiver);
            void *votes = (void *) mapGet(stateData->citizenVote, &stateTaker);
            int numVotes;
            if (votes == NULL) {
                numVotes = 1;
                MapResult result = mapPut(stateData->citizenVote, &stateTaker, &numVotes);
                if (result == MAP_OUT_OF_MEMORY) {
                    eurovisionDestroy(eurovision);
                    return EUROVISION_OUT_OF_MEMORY;
                }
            } else {
                numVotes = *(int *) votes;
                MapResult result = mapPut(stateData->citizenVote, &stateTaker, &numVotes + 1);
                if (result == MAP_OUT_OF_MEMORY) {
                    eurovisionDestroy(eurovision);
                    return EUROVISION_OUT_OF_MEMORY;
                }
                return EUROVISION_SUCCESS;
            }
        }

        EurovisionResult eurovisionRemoveVote(Eurovision eurovision, int stateGiver, int stateTaker)
        {
            if((!checkIfNotNegitive(stateGiver)) || (!checkIfNotNegitive(stateTaker))){
                return EUROVISION_INVALID_ID;
            }else if(stateGiver == stateTaker){
                return EUROVISION_SAME_STATE;
            }else{
                bool result1 = mapContains(eurovision->state,&stateGiver);
                bool result2 = mapContains(eurovision->state,&stateTaker);
                if((!result1) || (!result2)){
                    return EUROVISION_STATE_NOT_EXIST;
                }
            }
            StateData stateData = (StateData) mapGet(eurovision->state,&stateGiver);
            void*  votes = (void*) mapGet(stateData->citizenVote,&stateTaker);
            int numVotes;
            if(votes == NULL){
                return EUROVISION_SUCCESS;
            }else {
                numVotes = *(int *) votes;
                MapResult result = mapPut(stateData->citizenVote, &stateTaker, &numVotes - 1);
                if (result == MAP_OUT_OF_MEMORY) {
                    eurovisionDestroy(eurovision);
                    return EUROVISION_OUT_OF_MEMORY;
                }
                return EUROVISION_SUCCESS;
            }
        }

        List eurovisionRunContest(Eurovision eurovision, int audiencePercent);

        List eurovisionRunAudienceFavorite(Eurovision eurovision);

        List eurovisionRunGetFriendlyStates(Eurovision eurovision) {
        }

