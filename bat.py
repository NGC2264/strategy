import numpy as np
import pandas as pd

# 电池参数
PB_max = 100  # 最大充放电功率 (kW)
PB_min = 20   # 最小充放电功率 (kW)
SOC_max = 0.9 # 最大SOC
SOC_min = 0.3 # 最小SOC
battery_capacity = 500  # 电池容量 (Ah)
hours_per_day = 24
days_in_month = 30  # 假设一个月有30天

# 初始化数组来存储每天的充放电功率和SOC
discharge_powers = np.zeros((days_in_month, hours_per_day))
charge_powers = np.zeros((days_in_month, hours_per_day))
SOCs = np.zeros((days_in_month, hours_per_day))  # 加1是因为要检查结束和开始的SOC是否一致

# 设置初始SOC为满电状态
SOCs[0, 0] = 1

# 模拟一个月的充放电数据
for day in range(days_in_month):
    discharge_powers[day, :] = np.random.uniform(PB_min, PB_max, hours_per_day)
    charge_powers[day, :] = np.random.uniform(PB_min, PB_max, hours_per_day)
    
    SOC = SOCs[day, -1]  # 初始化当天的SOC为前一天的结束SOC
    
    for hour in range(hours_per_day):
        delta_SOC = (charge_powers[day, hour] - discharge_powers[day, hour]) / battery_capacity
        SOC += delta_SOC
        SOC = np.clip(SOC, SOC_min, SOC_max)
        if hour!=hours_per_day-1:
            SOCs[day, hour + 1] = SOC
        
    if day!=days_in_month-1:
        SOCs[day + 1, 0] = SOCs[day, -1]  # 应用周期性调度约束



# 将数组转换为DataFrame
df = pd.DataFrame({
    'Discharge Power': discharge_powers.flatten(),
    'Charge Power': charge_powers.flatten(),
    'SOC': SOCs.flatten()
})

# 为DataFrame添加时间索引
df.index = pd.date_range(start='2024-05-01', periods=days_in_month * hours_per_day, freq='H')

# 显示DataFrame
print(df.head())








# 雨流计数法分析
def rainflow_method(data):
    # 初始化变量
    cycles = []
    n = len(data)
    i = 0
    
    # 遍历数据
    while i < n - 1:
        start = i
        while i < n - 1 and data[i+1] >= data[i]:
            i += 1
        end = i
        if end == start:
            i += 1
            continue
        
        # 寻找循环的返回点
        while i < n - 1 and data[i+1] <= data[start]:
            i += 1
        
        # 计算循环的幅度
        cycle_amplitude = data[end] - data[start]
        cycles.append(cycle_amplitude)
        i += 1
    
    return cycles


soc_cycles=rainflow_method(df['SOC'])






##############################################################


# 经验参数
a = 1.0  # 可根据实际情况调整
b = 0.1  # 可根据实际情况调整
c = 0.0  # 可根据实际情况调整

# 计算储能寿命
Nlife = a * np.exp(-b * np.array(soc_cycles)) + c



plt.figure(figsize=(14, 7))
plt.plot(soc_cycles, Nlife, label='SOC')
plt.title('S-N')
plt.xlabel('amplitude-soc')
plt.ylabel('bat-life')
plt.legend()


plt.show()
###############################################################


def rainflow_method(df):
    # 提取充放电功率数据
    discharge_power = df['Discharge Power'].values
    charge_power = df['Charge Power'].values
    hours = len(discharge_power)
    
    # 初始化循环计数数组
    cycles = np.zeros(hours - 2, dtype=int)
    
    # 应用三点雨流计数法
    for i in range(1, hours - 1):
        if discharge_power[i - 1] < discharge_power[i] and discharge_power[i] > discharge_power[i + 1]:
            # 峰值
            cycles[i - 1] += 1
        elif discharge_power[i - 1] > discharge_power[i] and discharge_power[i] < discharge_power[i + 1]:
            # 谷值
            cycles[i - 1] -= 1
    
    # 计算DOD循环次数
    dod_cycles = np.abs(cycles).sum()
    return dod_cycles

# 使用上面定义的函数计算DOD循环次数
dod_cycles = rainflow_method(df)
print(f'Total DOD cycles: {dod_cycles}')