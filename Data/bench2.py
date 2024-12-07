import os
import csv
import pandas as pd

# Define the folders containing the log files
folders = [

    "hybrid-sa-pso30",
    "hybrid-sa-pso50",
    "hybrid-sa-pso70",

    ]

# Define a function to convert log files to txt files and process them
def process_log_files(folder):
    log_file_path = os.path.join("..", folder, f"{folder}.log")

    # Process the .txt file and save the results to .csv file
    csv_file_path = f"{folder}.csv"
    with open(log_file_path, 'r') as f, open(csv_file_path, 'w', newline='') as csvfile:
        fieldnames = ['Seed', 'Solucion Empleada', 'Problema', 'Tiempo', 'Nodos']
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()

        data = {'Solucion Empleada': folder.capitalize().replace("Joacapital", "JOA Capital")}
        for line in f:
            if "random seed:" in line:
                data['Seed'] = line.split()[-1]
            elif "file loaded:" in line:
                data['Problema'] = line.split('/')[-1].split('.')[0]
            elif "cpu time used:" in line:
                data['Tiempo'] = line.split()[-1][:-1]  # remove 's' from the end
            elif "number of cells:" in line:
                data['Nodos'] = line.split()[-1]
                writer.writerow(data)  # write data to csv and reset for next set
                data = {'Solucion Empleada': folder.capitalize().replace("Joacapital", "JOA Capital")}
    return csv_file_path

# Process all folders and collect csv file paths
csv_files = [process_log_files(folder) for folder in folders]

# Combine all CSV files into one and calculate averages
combined_df = pd.concat([pd.read_csv(csv_file) for csv_file in csv_files])

# Calculate averages and round to 2 decimal places
df_avg = combined_df.groupby(['Solucion Empleada', 'Problema']).agg({'Tiempo': 'mean', 'Nodos': 'mean'}).round(2).reset_index()

# Pivot the DataFrame
df_pivot = df_avg.pivot_table(index='Problema', columns='Solucion Empleada', values=['Tiempo', 'Nodos'])

# Create a DataFrame for times and nodes
df_times = df_pivot['Tiempo']
df_nodes = df_pivot['Nodos']

# Create a Pandas Excel writer using XlsxWriter as the engine.
writer = pd.ExcelWriter('combined_averages2.xlsx', engine='xlsxwriter')

# Write the time DataFrame to the first sheet
df_times.to_excel(writer, sheet_name='Tiempos')

# Write the nodes DataFrame to the second sheet
df_nodes.to_excel(writer, sheet_name='Nodos')

# Get the xlsxwriter workbook and worksheet objects.
workbook  = writer.book

# Get the xlsxwriter workbook and worksheet objects.
workbook  = writer.book

# Define a format for the minimum value in a row.
min_format = workbook.add_format({'bg_color': '#C6EFCE'})

# Apply the conditional format to the cells in the 'Tiempos' and 'Nodos' worksheets.
for sheet_name, df in zip(['Tiempos', 'Nodos'], [df_times, df_nodes]):
    worksheet = writer.sheets[sheet_name]
    for row in range(1, len(df) + 1):  # Skip the header row
        min_col = df.iloc[row - 1].idxmin()
        min_col_idx = df.columns.get_loc(min_col) + 2  # +1 for zero-indexing, +1 for skipping index column
        # Apply the conditional formatting rule to the cell with the minimum value in the row.
        worksheet.conditional_format(f'{chr(64 + min_col_idx)}{row + 1}', {'type': 'no_errors', 'format': min_format})

# Close the Pandas Excel writer and output the Excel file.
writer.close()

print("Data saved to combined_averages.xlsx")

#docker-compose up simulated-annealing simulated-annealing-joa hill-climbing abstaylor -d --build
#docker-compose up hill-climbing-parallel -d --build
#docker-compose up simulated-annealing-parallel -d --build
#docker-compose up simulated-annealing-joa-parallel -d --build
#python Data/bench.py

