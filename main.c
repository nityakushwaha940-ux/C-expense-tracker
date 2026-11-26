#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 50
#define MAX_RECORDS 200

typedef struct {
    int id;
    char date[15];
    char category[MAX_STR];
    char description[MAX_STR];
    float amount;
} Expense;

// Storage state (supports file I/O with automatic fallback for online sandboxes)
char activeFile[64] = "expenses.dat";
int useMemoryOnly = 0;
Expense memRecords[MAX_RECORDS];
int memCount = 0;

// Function prototypes
void initStorage();
void addExpense();
void viewExpenses();
void searchByCategory();
void deleteExpense();
void budgetSummary();
int getNextId();
void clearInputBuffer();
int matchCategory(const char *a, const char *b);

int main() {
    int choice;
    initStorage();

    while (1) {
        printf("\n==========================================\n");
        printf("     PERSONAL EXPENSE & BUDGET TRACKER    \n");
        printf("==========================================\n");
        printf("1. Add New Expense\n");
        printf("2. View All Expenses\n");
        printf("3. Search Expenses by Category\n");
        printf("4. Delete an Expense\n");
        printf("5. Budget & Spending Summary\n");
        printf("6. Exit\n");
        printf("------------------------------------------\n");
        printf("Enter your choice (1-6): ");

        if (scanf("%d", &choice) != 1) {
            printf("\n[Error] Invalid input. Please enter a number.\n");
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1:
                addExpense();
                break;
            case 2:
                viewExpenses();
                break;
            case 3:
                searchByCategory();
                break;
            case 4:
                deleteExpense();
                break;
            case 5:
                budgetSummary();
                break;
            case 6:
                printf("\nExiting program. Goodbye!\n");
                exit(0);
            default:
                printf("\n[Error] Please choose a valid option (1-6).\n");
        }
    }
    return 0;
}

// Check writable file location or switch to memory mode if sandboxed
void initStorage() {
    FILE *fp = fopen("expenses.dat", "ab");
    if (fp != NULL) {
        fclose(fp);
        strcpy(activeFile, "expenses.dat");
        return;
    }

    fp = fopen("/tmp/expenses.dat", "ab");
    if (fp != NULL) {
        fclose(fp);
        strcpy(activeFile, "/tmp/expenses.dat");
        return;
    }

    useMemoryOnly = 1;
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int matchCategory(const char *a, const char *b) {
    #ifdef _WIN32
    return _stricmp(a, b) == 0;
    #else
    return strcasecmp(a, b) == 0;
    #endif
}

int getNextId() {
    int maxId = 0;

    if (useMemoryOnly) {
        for (int i = 0; i < memCount; i++) {
            if (memRecords[i].id > maxId) maxId = memRecords[i].id;
        }
        return maxId + 1;
    }

    FILE *fp = fopen(activeFile, "rb");
    if (fp == NULL) return 1;

    Expense exp;
    while (fread(&exp, sizeof(Expense), 1, fp)) {
        if (exp.id > maxId) maxId = exp.id;
    }
    fclose(fp);
    return maxId + 1;
}

// 1. Add a new expense
void addExpense() {
    Expense exp;
    exp.id = getNextId();

    printf("\n--- Add New Expense (ID: %d) ---\n", exp.id);
    printf("Enter Date (DD-MM-YYYY): ");
    fgets(exp.date, sizeof(exp.date), stdin);
    exp.date[strcspn(exp.date, "\n")] = 0;

    printf("Enter Category (e.g., Food, Travel, Bills): ");
    fgets(exp.category, sizeof(exp.category), stdin);
    exp.category[strcspn(exp.category, "\n")] = 0;

    printf("Enter Description: ");
    fgets(exp.description, sizeof(exp.description), stdin);
    exp.description[strcspn(exp.description, "\n")] = 0;

    printf("Enter Amount: ");
    while (scanf("%f", &exp.amount) != 1 || exp.amount < 0) {
        printf("[Error] Enter a valid positive amount: ");
        clearInputBuffer();
    }
    clearInputBuffer();

    if (useMemoryOnly) {
        if (memCount < MAX_RECORDS) {
            memRecords[memCount++] = exp;
            printf("\n[Success] Expense recorded successfully!\n");
        } else {
            printf("\n[Error] Storage limit reached.\n");
        }
        return;
    }

    FILE *fp = fopen(activeFile, "ab");
    if (fp == NULL) {
        printf("\n[Error] Unable to open file for writing.\n");
        return;
    }

    fwrite(&exp, sizeof(Expense), 1, fp);
    fclose(fp);
    printf("\n[Success] Expense recorded successfully!\n");
}

// 2. Display all recorded expenses
void viewExpenses() {
    Expense exp;
    int count = 0;
    float total = 0.0;

    printf("\n--------------------------------------------------------------------------------\n");
    printf("%-5s %-12s %-15s %-28s %-10s\n", "ID", "Date", "Category", "Description", "Amount");
    printf("--------------------------------------------------------------------------------\n");

    if (useMemoryOnly) {
        for (int i = 0; i < memCount; i++) {
            printf("%-5d %-12s %-15s %-28s %.2f\n",
                   memRecords[i].id, memRecords[i].date, memRecords[i].category,
                   memRecords[i].description, memRecords[i].amount);
            total += memRecords[i].amount;
            count++;
        }
    } else {
        FILE *fp = fopen(activeFile, "rb");
        if (fp != NULL) {
            while (fread(&exp, sizeof(Expense), 1, fp)) {
                printf("%-5d %-12s %-15s %-28s %.2f\n",
                       exp.id, exp.date, exp.category, exp.description, exp.amount);
                total += exp.amount;
                count++;
            }
            fclose(fp);
        }
    }

    printf("--------------------------------------------------------------------------------\n");
    if (count == 0) {
        printf("No records to display.\n");
    } else {
        printf("Total Records: %d | Total Expenditure: %.2f\n", count, total);
    }
}

// 3. Search expenses by category
void searchByCategory() {
    char target[MAX_STR];
    printf("\nEnter Category to search: ");
    fgets(target, sizeof(target), stdin);
    target[strcspn(target, "\n")] = 0;

    Expense exp;
    int found = 0;
    float categoryTotal = 0.0;

    printf("\n--- Results for Category: '%s' ---\n", target);
    printf("%-5s %-12s %-28s %-10s\n", "ID", "Date", "Description", "Amount");
    printf("-------------------------------------------------------------\n");

    if (useMemoryOnly) {
        for (int i = 0; i < memCount; i++) {
            if (matchCategory(memRecords[i].category, target)) {
                printf("%-5d %-12s %-28s %.2f\n",
                       memRecords[i].id, memRecords[i].date,
                       memRecords[i].description, memRecords[i].amount);
                categoryTotal += memRecords[i].amount;
                found++;
            }
        }
    } else {
        FILE *fp = fopen(activeFile, "rb");
        if (fp != NULL) {
            while (fread(&exp, sizeof(Expense), 1, fp)) {
                if (matchCategory(exp.category, target)) {
                    printf("%-5d %-12s %-28s %.2f\n", exp.id, exp.date, exp.description, exp.amount);
                    categoryTotal += exp.amount;
                    found++;
                }
            }
            fclose(fp);
        }
    }

    if (found == 0) {
        printf("No expenses found under '%s'.\n", target);
    } else {
        printf("-------------------------------------------------------------\n");
        printf("Category Total (%d items): %.2f\n", found, categoryTotal);
    }
}

// 4. Delete an expense by ID
void deleteExpense() {
    int targetId, found = 0;
    printf("\nEnter the Expense ID to delete: ");
    if (scanf("%d", &targetId) != 1) {
        printf("[Error] Invalid ID.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    if (useMemoryOnly) {
        for (int i = 0; i < memCount; i++) {
            if (memRecords[i].id == targetId) {
                found = 1;
                for (int j = i; j < memCount - 1; j++) {
                    memRecords[j] = memRecords[j + 1];
                }
                memCount--;
                break;
            }
        }
    } else {
        FILE *fp = fopen(activeFile, "rb");
        if (fp == NULL) {
            printf("\n[Info] No expense records found.\n");
            return;
        }

        char tempFile[80];
        snprintf(tempFile, sizeof(tempFile), "%s.tmp", activeFile);
        FILE *tempFp = fopen(tempFile, "wb");
        if (tempFp == NULL) {
            printf("[Error] Could not create temporary file.\n");
            fclose(fp);
            return;
        }

        Expense exp;
        while (fread(&exp, sizeof(Expense), 1, fp)) {
            if (exp.id == targetId) {
                found = 1;
            } else {
                fwrite(&exp, sizeof(Expense), 1, tempFp);
            }
        }
        fclose(fp);
        fclose(tempFp);

        remove(activeFile);
        rename(tempFile, activeFile);
    }

    if (found) {
        printf("\n[Success] Expense ID %d deleted.\n", targetId);
    } else {
        printf("\n[Info] Expense ID %d not found.\n", targetId);
    }
}

// 5. Compare total spending against a budget limit
void budgetSummary() {
    float budget;
    printf("\nEnter your target budget limit: ");
    if (scanf("%f", &budget) != 1 || budget <= 0) {
        printf("[Error] Invalid budget amount.\n");
        clearInputBuffer();
        return;
    }
    clearInputBuffer();

    float totalSpent = 0.0;
    int hasRecords = 0;

    if (useMemoryOnly) {
        hasRecords = (memCount > 0);
        for (int i = 0; i < memCount; i++) {
            totalSpent += memRecords[i].amount;
        }
    } else {
        FILE *fp = fopen(activeFile, "rb");
        if (fp != NULL) {
            Expense exp;
            while (fread(&exp, sizeof(Expense), 1, fp)) {
                totalSpent += exp.amount;
                hasRecords = 1;
            }
            fclose(fp);
        }
    }

    if (!hasRecords) {
        printf("\n[Info] No expense records found.\n");
        return;
    }

    printf("\n--- Budget Analysis ---\n");
    printf("Target Budget : %.2f\n", budget);
    printf("Total Spent   : %.2f\n", totalSpent);

    if (totalSpent > budget) {
        printf("Status        : OVER BUDGET by %.2f!\n", totalSpent - budget);
    } else {
        printf("Status        : WITHIN BUDGET (Remaining: %.2f)\n", budget - totalSpent);
    }
}
