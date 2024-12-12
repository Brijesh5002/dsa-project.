#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_INGREDIENTS 100
#define MAX 100

//FUNCTION PROTOTYPES
struct RecipeNode *createRecipeNode(const char *title, const char *instructions);
struct IngredientNode *createIngredientNode(const char *name);
struct RecipeNode *loadRecipes(const char *recipes);
void findRecipes(struct RecipeNode *recipes, struct IngredientNode *availableIngredients);
int canMakeRecipes(struct RecipeNode *recipe, struct IngredientNode *availableIngredients);
void freeIngredients(struct IngredientNode *start);
void freeRecipes(struct RecipeNode *start);

// INGREDIENT NODE
struct IngredientNode
{
    char name[MAX];
    struct IngredientNode *next;
};
struct IngredientNode *ingredient_node_start = 0;

// RECIPE NODE
struct RecipeNode
{
    char title[MAX];
    char instructions[1000];
    struct IngredientNode *ingredients;
    struct RecipeNode *next;
};
struct RecipeNode *recipe_node_start = 0;

// GRAPH NODE
struct GraphNode
{
    struct RecipeNode *recipe;
    struct GraphNode *next;
};
struct GraphNode *graph_node_start = 0;

// FUNCTION TO CREATE NEW INGREDIENT NODE
struct IngredientNode *createIngredientNode(const char *name)
{
    struct IngredientNode *new_node = (struct IngredientNode *)malloc(sizeof(struct IngredientNode));
    strcpy(new_node->name, name);
    new_node->next = 0;
    return new_node;
}

// Function to create a recipe node
struct RecipeNode* createRecipeNode(const char *title, const char *instructions) {
    struct RecipeNode *node = (struct RecipeNode *)malloc(sizeof(struct RecipeNode));
    strcpy(node->title, title);
    strcpy(node->instructions, instructions);
    node->ingredients = NULL;
    node->next = NULL;
    return node;
}
// FUNCTION TO CREATE A NEW RECIPE NODE
struct RecipeNode *loadRecipes(const char *recipes)
{
    FILE *file = fopen(recipes, "r");
    if (file == NULL)
    {
        printf("Could not open file %s\n", recipes);
        return NULL;
    }

    struct RecipeNode *start = NULL, *tail = NULL;
    char line[MAX];

    while (fgets(line, sizeof(line), file))
    {
        char title[MAX];
        char instructions[4000];

        // Read TITLE
        sscanf(line, "%[^\n]", title);
        struct RecipeNode *recipe = createRecipeNode(" ", " "); // Create a new recipe node

        // Copy the title into the recipe node
        strcpy(recipe->title, title);

        // Read INSTRUCTIONS
        fgets(instructions, sizeof(instructions), file);
        strcpy(recipe->instructions, instructions);

        // READ INGREDIENTS
        while (fgets(line, sizeof(line), file) && strcmp(line, "\n") != 0)
        {
            line[strcspn(line, "\n")] = 0; // Remove newline character
            struct IngredientNode *ingredient = createIngredientNode(line);
            ingredient->next = recipe->ingredients;
            recipe->ingredients = ingredient;
        }

        // ADD RECIPES TO THE LIST
        if (start == NULL)
        {
            start = recipe;
            tail = recipe;
        }
        else
        {
            tail->next = recipe;
            tail = recipe;
        }
    }

    fclose(file); // Close the file pointer
    return start; // Return the head of the linked list of recipes
}

void findRecipes(struct RecipeNode *recipes, struct IngredientNode *availableIngredients)
{
    struct RecipeNode *currentRecipe = recipes;
    printf("Recipies that you can make:\n");
    while (currentRecipe != NULL)
    {
        struct IngredientNode *currentIngredient = currentRecipe->ingredients;
        int found = 1;
        while (currentIngredient != NULL)
        {
            struct IngredientNode *available = availableIngredients;
            int ingredientFound = 0;

            while (available != NULL)
            {
                if (strcmp(currentIngredient->name, available->name) == 0)
                {
                    ingredientFound = 1;

                    break;
                }
                available = available->next;
            }
            if (ingredientFound == 0)
            {
                found = 0;

                break;
            }
            currentIngredient = currentIngredient->next;
        }
        if (found == 1)
        {
            printf("%s\n", currentRecipe->title);
        }

        currentRecipe = currentRecipe->next;
    }
}

//The function canMakeRecipe is designed to determine whether a specific recipe can be made based on the available ingredients. 
int canMakeRecipes(struct RecipeNode*recipe,struct IngredientNode *availableIngredients){
    struct IngredientNode*ingredient= recipe->ingredients;
    while(ingredient!=NULL){
        struct IngredientNode*available=availableIngredients;
        int found=0;
        while(available!=NULL){
            if(strcmp(ingredient->name,available->name)==0){
                found=1;
                break;
            }
            available=available->next;
            }
            if (found==0){
                return 0;  //ingredient not found
            }   
            \
            ingredient=ingredient->next;
    }
return 1;      //all ingredients found
}


// Free allocated memory for ingredients
void freeIngredients(struct IngredientNode *start) {
    struct IngredientNode *temp;
    while (start) {
        temp = start;
        start = start->next;
        free(temp);
    }
}

// Free allocated memory for recipes
void freeRecipes(struct RecipeNode *start) {
    struct RecipeNode *temp;
    while (start) {
        freeIngredients(start->ingredients); // Free ingredients of the recipe
        temp = start;
        start = start->next;
        free(temp);
    }
}

//only main function is left
int main() {
    // Create a linked list of ingredients
    struct IngredientNode *availableIngredients = NULL;
    struct IngredientNode *tail = NULL;

    // User input for available ingredients
    printf("Enter available ingredients (type 'done' to finish):\n");
    char name[MAX];
    while (1) {
        fgets(name, sizeof(name), stdin);
        name[strcspn(name, "\n")] = 0;  
        if (strcmp(name, "done") == 0) {
            break;
        }
        struct IngredientNode *newIngredient = createIngredientNode(name);
        if (availableIngredients == NULL) {
            availableIngredients = newIngredient;
            tail = newIngredient;
        } else {
            tail->next = newIngredient;
            tail = newIngredient;
        }
    }

    // Load recipes from file
    struct RecipeNode *recipes = loadRecipes("recipes.txt");
    if (recipes == NULL) {
        printf("No recipes loaded.\n");
        freeIngredients(availableIngredients);
        return 0;
    }

    if (availableIngredients == NULL) {
        printf("No available ingredients\n");
        freeRecipes(recipes);
        return 0;
    }

    // Find and display recipes that can be made
    findRecipes(recipes, availableIngredients);

    // Free allocated memory
    freeIngredients(availableIngredients);
    freeRecipes(recipes);
    return 0;
}