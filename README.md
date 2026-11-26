# C-expense-tracker
# Personal Expense & Budget Tracker in C

A menu-driven command-line application built in C to record, categorize, search, delete, and analyze daily expenses against a target budget.

## Features
- **Add New Expense:** Record date, category, description, and amount with auto-generated IDs.
- **View All Expenses:** Displays a formatted table of all saved transactions along with total expenditure.
- **Search by Category:** Case-insensitive filtering by category with category-wise spending totals.
- **Delete Expense:** Remove specific records cleanly by ID.
- **Budget & Spending Summary:** Compares total spending against a user-defined budget limit and alerts if over budget.

## Key C Concepts & Functions Used
- **Structures (`typedef struct`):** Groups mixed data types (`id`, `date`, `category`, `description`, `amount`) into an `Expense` record.
- **Binary File Handling (`FILE*`):** Uses `fopen()`, `fwrite()`, `fread()`, `fclose()`, `remove()`, and `rename()` for persistent storage (`expenses.dat`) with an automatic fallback for restricted sandbox environments.
- **Safe Input & String Handling:** Uses `fgets()`, `strcspn()`, `strcpy()`, and `strcasecmp()` / `_stricmp()` to prevent buffer overflows and handle multi-word inputs.

## How to Compile and Run

```bash
gcc main.c -o expense_tracker
./expense_tracker
