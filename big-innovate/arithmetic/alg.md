# 推荐算法概述

## 1. 基于热度的推荐算法

[热度的推荐算法](http://pogf.com.cn:8080/heat)

```go
r.GET("/click", func(c *gin.Context) {
		buttonID := c.DefaultQuery("button_id", "")
		if buttonID == "" {
			c.JSON(http.StatusBadRequest, gin.H{"error": "Button ID not provided"})
			return
		}

		// 尝试从Redis中获取按钮点击次数
		countKey := fmt.Sprintf("button:%s:count", buttonID)
		ctx := context.Background()
		countStr, err := rdb.Get(ctx, countKey).Result()
		if err != nil && err != redis.Nil {
			c.JSON(http.StatusInternalServerError, gin.H{"error": "Error fetching click count"})
			return
		}

		var count int64
		if err == redis.Nil {
			count = 0
		} else {
			count, _ = strconv.ParseInt(countStr, 10, 64)
		}

		// 增加点击次数并更新到Redis
		count++
		err = rdb.Set(ctx, countKey, strconv.FormatInt(count, 10), 0).Err()
		if err != nil {
			c.JSON(http.StatusInternalServerError, gin.H{"error": "Error updating click count"})
			return
		}

		c.JSON(http.StatusOK, gin.H{"message": "Click recorded", "count": count})
	})

```


```go

r.GET("/leaderboard", func(c *gin.Context) {
		// 从Redis获取所有按钮的点击次数并排序
		ctx := context.Background()
		keys := rdb.Keys(ctx, "button:*:count").Val()
		leaderboard := make([]map[string]interface{}, len(keys))
		for i, key := range keys {
			countStr, err := rdb.Get(ctx, key).Result()
			if err != nil {
				continue
			}
			buttonID := strings.Split(key, ":")[1]
			count, _ := strconv.ParseInt(countStr, 10, 64)
			leaderboard[i] = map[string]interface{}{"buttonId": buttonID, "count": count}
		}
		sort.SliceStable(leaderboard, func(i, j int) bool {
			return leaderboard[i]["count"].(int64) > leaderboard[j]["count"].(int64)
		})
		c.JSON(http.StatusOK, leaderboard)
	})

```

```html	
<!DOCTYPE html>
<html lang="zh-cn">
<head>
    <title>Button Click Counter</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f4;
            text-align: center;
        }

        h1 {
            color: #333;
        }

        .button-container {
            display: flex;
            justify-content: center;
            align-items: center;
            flex-wrap: wrap;
            margin-top: 20px;
        }

        .custom-button {
            padding: 10px 20px;
            margin: 10px;
            font-size: 16px;
            background-color: #007bff;
            color: #fff;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            transition: background-color 0.3s;
        }

        .custom-button:hover {
            background-color: #0056b3;
        }

        .click-count {
            font-size: 1.5em;
            font-weight: bold;
            margin:  30px 0;
        }
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f4;
            text-align: center;
        }

        /* ... 其他样式 ... */

        .leaderboard {
            max-width: 600px;
            margin: 0 auto;
            padding: 20px;
            background-color: #fff;
            border-radius: 10px;
            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.1);
        }

        .leaderboard h2 {
            color: #333;
            margin-bottom: 10px;
        }

        .leaderboard-item {
            display: flex;
            justify-content: space-between;
            align-items: center;
            padding: 10px;
            border-top: 1px solid #ddd;
            transition: background-color 0.2s;
        }

        .leaderboard-item:hover {
            background-color: #f7f7f7;
        }

        .leaderboard-item span {
            font-size: 16px;
        }

        .leaderboard-rank {
            font-weight: bold;
            margin-right: 10px;
        }

        .leaderboard-buttonId {
            flex-grow: 1;
            margin-right: 10px;
        }

        .leaderboard-count {
            color: #007bff;
            font-weight: bold;
        }
        .leaderboard-item.first-place {
            background-color: #ff6347; /* 设置第一名的背景颜色 */
            font-weight: bold;
        }

        .leaderboard-item.second-place {
            background-color: #ffa500; /* 设置第二名的背景颜色 */
            font-weight: bold;
        }

        .leaderboard-item.third-place {
            background-color: #ffd700; /* 设置第三名的背景颜色 */
            font-weight: bold;
        }

    </style>
    <script src="https://code.jquery.com/jquery-3.6.0.min.js"></script>
</head>
<body>
<h1>Button Click Counter</h1>
<div class="button-container">
    <!-- Generate 10 buttons with IDs video-1 to video-10 -->
    <!-- You can use a loop to generate the buttons -->
    <button class="custom-button" id="video-1">Video  1</button>
    <button class="custom-button" id="video-2">Video  2</button>
    <button class="custom-button" id="video-3">Video  3</button>
    <button class="custom-button" id="video-4">Video  4</button>
    <button class="custom-button" id="video-5">Video  5</button>
    <button class="custom-button" id="video-6">Video  6</button>
    <button class="custom-button" id="video-7">Video  7</button>
    <button class="custom-button" id="video-8">Video  8</button>
    <button class="custom-button" id="video-9">Video  9</button>
    <button class="custom-button" id="video-10">Video  10</button>
</div>
<div class="click-count">
    Click Count: <span id="count">0</span>
</div>
<div class="leaderboard">
    <h2>Click Count Leaderboard</h2>
    <ol id="leaderboard-list" class="leaderboard-list">
        <!-- Leaderboard items will be added dynamically -->
    </ol>
</div>

<script>
    $(document).ready(function () {
        // Loop through all buttons and add click event handler
        for (let i = 1; i <= 10; i++) {
            const buttonId = `video-${i}`;
            $(`#${buttonId}`).click(function () {
                $.get("/click", {button_id: buttonId}, function (data) {
                    $(`#${buttonId}`).addClass("clicked");
                    $("#count").text(data.count);
                });
            });
        }

        function updateLeaderboard() {
            $.get("/leaderboard", function(data) {
                $("#leaderboard-list").empty();
                $.each(data, function(index, item) {
                    const rankClass = index === 0 ? "first-place" : (index === 1 ? "second-place" : (index === 2 ? "third-place" : ""));
                    const leaderboardItem = `
                        <li class="leaderboard-item ${rankClass}">
                            <span class="leaderboard-rank">${index + 1}.</span>
                            <span class="leaderboard-buttonId">${item.buttonId}</span>
                            <span class="leaderboard-count">${item.count}</span>
                        </li>`;
                    $("#leaderboard-list").append(leaderboardItem);
                    // Add a small delay for the transition effect
                    setTimeout(function() {
                        $(".leaderboard-item").addClass("transitioned");
                    }, 1000);
                });
            });
        }


        // Initial leaderboard update
        updateLeaderboard();

        // Refresh leaderboard every 5 seconds
        setInterval(updateLeaderboard,3000)
    });

</script>
</body>
</html>

```

## 2. 协同过滤

### 1. 余弦相似度

余弦相似度（Cosine Similarity）是一种用于衡量两个向量之间相似程度的计算方法。它常用于文本、图像和推荐系统等领域，用于比较多维空间中的向量之间的夹角，判断它们的方向是否相似。余弦相似度计算的是两个向量之间的夹角的余弦值，这个值越接近1，表示两个向量越相似。

### 2. 推荐算法中的应用

在推荐系统中，余弦相似度常用于计算用户之间的相似度或物品之间的相似度，从而进行推荐。具体来说，对于用户推荐，可以计算用户喜好的向量之间的相似度，找出与目标用户喜好相似的其他用户，然后根据这些相似用户的喜好进行推荐。对于物品推荐，可以计算不同物品的特征向量之间的相似度，找出与目标物品相似的其他物品，然后将这些相似物品推荐给用户。

[余弦相似度](https://zh.wikipedia.org/wiki/%E4%BD%99%E5%BC%A6%E7%9B%B8%E4%BC%BC%E6%80%A7)

余弦相似度可以表示为：
$$
\text{similarity}=\text{cos}{(\Theta)} = \frac{\sum_{i=1}^{n} A_i \cdot B_i}{\sqrt{\sum_{i=1}^{n} A_i^2} \cdot \sqrt{\sum_{i=1}^{n} B_i^2}}
$$
余弦相似度的值范围在 -1 到 1 之间，其中：

- 值为 1 表示两个向量的方向完全一致，即完全相似；
- 值为 -1 表示两个向量的方向完全相反，即完全不相似；
- 值为 0 表示两个向量的方向垂直，即无关联。

### 3. 代码测试

```python	
import numpy as np
from sklearn.metrics.pairwise import cosine_similarity

# 假设视频总数为100，用户总数为6
num_videos = 100
num_users = 6

# 创建用户-视频交互矩阵
user_video_matrix = np.zeros((num_users, num_videos), dtype=int)

# 设置用户的喜好视频，确保每个用户喜欢的视频数量在70左右
num_liked_videos = 70

for user_id in range(num_users):
    liked_indices = np.random.choice(num_videos, size=num_liked_videos, replace=False)
    user_video_matrix[user_id, liked_indices] = 1
print("用户-视频交互矩阵：")
print(user_video_matrix)

# 将用户-视频交互矩阵转换为新的矩阵，每32位转为一个整数，并在不足32位时后面补零
num_bits = 32
num_rows = user_video_matrix.shape[0]
num_cols = (user_video_matrix.shape[1] + num_bits - 1) // num_bits  # 向上取整

new_matrix = np.zeros((num_rows, num_cols), dtype=np.uint32)

for row in range(num_rows):
    for col in range(num_cols):
        start_bit = col * num_bits
        end_bit = (col + 1) * num_bits
        bits = user_video_matrix[row, start_bit:end_bit]
        bits = np.pad(bits, (0, num_bits - len(bits)), mode='constant')
        new_matrix[row, col] = int(''.join(map(str, bits)), 2)

print("转换后的矩阵：")
print(new_matrix)
# 计算用户之间的余弦相似度
user_similarity = cosine_similarity(user_video_matrix)
print(user_similarity)
# 选择近邻用户数量
num_neighbors = 6

# 基于用户相似性选择近邻用户
top_neighbors = np.argsort(user_similarity, axis=1)[:, -num_neighbors-1:-1]  # 不包括自己

# 生成推荐
recommendations = []
for user_id in range(num_users):
    not_watched = np.where(user_video_matrix[user_id] == 0)[0]  # 找出未观看的视频
    print(not_watched)
    recommended_videos = np.isin(top_neighbors[user_id], not_watched)  # 找出近邻用户喜欢的未观看视频
    recommendations.append(np.where(recommended_videos)[0])

# 打印推荐结果
for user_id, rec_videos in enumerate(recommendations):
    print(f"用户{user_id}的推荐视频数：{len(rec_videos)}")
    # 打印前10个推荐视频
    print(f"用户{user_id}的前10个推荐视频：{rec_videos[:10]}")
    print()

```

```tex
用户-视频交互矩阵：
[[1 1 1 1 0 0 1 1 1 1 1 0 0 0 0 0 0 1 1 1 1 1 0 1 1 0 1 1 1 1 1 1 1 1 1 0
  1 1 1 1 1 1 1 0 1 1 1 1 1 0 0 1 0 1 1 1 1 1 1 1 1 1 1 1 0 1 1 1 0 1 0 1
  1 0 0 1 1 0 1 0 1 0 1 1 0 1 1 0 0 1 0 1 1 1 0 1 0 1 0 1]
 [1 1 1 0 1 0 1 1 1 1 1 1 0 1 1 1 0 0 1 1 1 1 1 1 1 1 1 0 1 0 1 1 1 0 1 1
  1 0 0 1 0 0 1 0 1 1 1 1 0 0 1 1 0 1 1 0 1 1 1 1 1 1 0 1 1 1 0 1 1 1 1 1
  1 0 0 1 0 0 0 1 1 1 0 1 0 1 0 0 1 1 1 1 1 0 1 0 1 1 1 1]
 [1 1 1 1 1 1 1 1 1 1 1 0 0 1 1 1 0 0 1 1 1 0 1 1 0 1 0 1 0 1 1 1 0 0 0 1
  1 1 1 1 1 0 0 1 1 1 1 0 1 1 1 1 0 1 1 1 0 1 0 1 0 0 1 1 1 1 1 0 1 1 0 1
  1 1 1 0 0 0 1 1 1 1 0 0 1 1 0 1 1 1 1 1 1 0 0 1 0 1 1 1]
 [0 1 1 1 0 1 1 1 1 0 1 1 1 1 1 1 1 1 1 0 1 0 1 0 0 1 1 1 0 1 1 0 1 0 1 0
  1 1 1 1 1 1 0 1 0 0 1 0 1 1 1 1 1 0 1 1 0 1 1 1 1 1 0 1 1 1 1 0 1 0 1 0
  1 1 0 1 1 0 0 1 1 1 1 1 1 1 0 1 1 0 1 0 0 1 1 1 0 1 1 0]
 [0 1 1 1 1 1 1 0 0 1 0 1 1 1 1 1 1 0 1 0 1 0 1 0 1 1 0 1 1 1 1 0 1 1 1 1
  0 1 0 1 1 1 0 1 1 0 0 1 0 0 1 1 1 1 1 1 1 1 1 1 1 0 0 0 1 1 1 0 1 1 1 1
  1 1 1 1 1 1 0 1 0 1 1 1 0 1 1 0 0 1 1 1 0 1 1 1 0 0 0 1]
 [1 1 1 0 1 0 0 1 1 1 0 1 1 0 1 0 1 1 1 1 1 0 1 1 0 1 1 1 1 1 1 1 0 0 0 1
  1 1 0 0 1 1 1 0 1 0 0 1 0 1 0 0 1 1 1 0 1 1 1 1 1 1 1 1 1 1 1 0 1 1 1 0
  1 1 1 0 1 1 0 0 1 1 0 1 1 1 0 1 1 1 0 1 1 1 1 1 1 0 1 0]]
转换后的矩阵：
[[4091575743 4025456639 1973073501 1342177280]
 [3958849515 3106879229 3750876410 4026531840]
 [4293344087  530511699 3991129593 1879048192]
 [2009066102 2949839741 3940154791 1610612736]
 [2120198878 4124655608 4026365559  268435456]
 [3923442559  485052159 4008500703 2684354560]]

```

```tex
[[1.         0.72857143 0.68571429 0.64285714 0.65714286 0.67142857]
 [0.72857143 1.         0.72857143 0.65714286 0.68571429 0.71428571]
 [0.68571429 0.72857143 1.         0.72857143 0.67142857 0.7       ]
 [0.64285714 0.65714286 0.72857143 1.         0.74285714 0.7       ]
 [0.65714286 0.68571429 0.67142857 0.74285714 1.         0.7       ]
 [0.67142857 0.71428571 0.7        0.7        0.7        1.        ]]
```

```shell
用户0的推荐视频数：2
用户0的前10个推荐视频：[0 2]

用户1的推荐视频数：2
用户1的前10个推荐视频：[0 2]

用户2的推荐视频数：1
用户2的前10个推荐视频：[1]

用户3的推荐视频数：0
用户3的前10个推荐视频：[]

用户4的推荐视频数：2
用户4的前10个推荐视频：[2 4]

用户5的推荐视频数：1
用户5的前10个推荐视频：[2]
```

## 3. 基于内容的推荐

```python
import numpy as np
from sklearn.metrics.pairwise import cosine_similarity

# 生成随机的视频特征向量和用户兴趣向量
np.random.seed(42)

num_videos = 100
num_users = 6
num_features = 5

videos = {}
for i in range(num_videos):
    video_name = f'Video{i+1}'
    features = np.random.randint(0, 2, num_features)  # 随机生成0和1
    videos[video_name] = features
print(videos)
users = {}
for i in range(num_users):
    user_name = f'User{i+1}'
    interests = np.random.randint(0, 2, num_features)
    users[user_name] = interests
print(users)
# 计算视频与用户兴趣的余弦相似度
similarities = {}
for video, features in videos.items():
    video_similarities = []
    for user, interests in users.items():
        sim = cosine_similarity([features], [interests])[0][0]
        video_similarities.append((user, sim))
    similarities[video] = video_similarities
print(similarities)
# 打印推荐结果
for video, video_similarities in similarities.items():
    print(f"{video} 的推荐：")
    sorted_similarities = sorted(video_similarities, key=lambda x: x[1], reverse=True)
    for user, similarity in sorted_similarities:
        print(f"   {user} - 相似度: {similarity}")

```



### 1. 模拟视频特征向量

```shell
{'Video1': array([0, 1, 0, 0, 0]), 'Video2': array([1, 0, 0, 0, 1]), 'Video3': array([0, 0, 0, 0, 1]), 'Video4': array([0, 1, 1, 1, 0]), 'Video5': array([1, 0, 1, 1, 1]), 'Video6': array([1, 1, 1, 1, 1]), 'Video7': array([0, 0, 1, 1, 1]), 'Video8': array([0, 1, 0, 0, 0]), 'Video9': array([0, 0, 1, 1, 1]), 'Video10': array([1, 1, 0, 1, 1]), 'Video11': array([0, 1, 0, 1, 0]), 'Video12': array([1, 1, 0, 0, 0]), 'Video13': array([0, 0, 0, 0, 0]), 'Video14': array([1, 1, 0, 1, 1]), 'Video15': array([1, 1, 0, 1, 0]), 'Video16': array([1, 1, 1, 0, 1]), 'Video17': array([0, 1, 0, 1, 0]), 'Video18': array([0, 1, 0, 1, 1]), 'Video19': array([1, 1, 1, 1, 1]), 'Video20': array([1, 1, 1, 1, 0]), 'Video21': array([0, 1, 1, 1, 1]), 'Video22': array([1, 1, 1, 1, 0]), 'Video23': array([1, 0, 1, 1, 0]), 'Video24': array([1, 0, 1, 1, 0]), 'Video25': array([1, 0, 1, 0, 0]), 'Video26': array([1, 1, 0, 1, 1]), 'Video27': array([1, 0, 0, 0, 0]), 'Video28': array([0, 0,
 0, 0, 0]), 'Video29': array([1, 0, 1, 1, 1]), 'Video30': array([0, 0, 0, 0, 1]), 'Video31': array([0, 0, 0, 0, 0]), 'Video32': array([1, 0, 1, 0, 1]), 'Video33
': array([0, 0, 1, 1, 1]), 'Video34': array([0, 1, 0, 0, 1]), 'Video35': array([1, 0, 0, 1, 1]), 'Video36': array([1, 0, 0, 0, 0]), 'Video37': array([0, 0, 1, 0
, 0]), 'Video38': array([0, 1, 0, 0, 1]), 'Video39': array([0, 0, 0, 0, 0]), 'Video40': array([1, 1, 1, 0, 0]), 'Video41': array([0, 1, 0, 0, 1]), 'Video42': ar
ray([0, 1, 1, 1, 0]), 'Video43': array([0, 0, 0, 0, 0]), 'Video44': array([0, 1, 0, 1, 0]), 'Video45': array([0, 0, 1, 1, 1]), 'Video46': array([1, 0, 1, 0, 0])
, 'Video47': array([1, 1, 1, 1, 1]), 'Video48': array([1, 1, 1, 0, 1]), 'Video49': array([1, 0, 1, 0, 0]), 'Video50': array([1, 0, 0, 0, 0]), 'Video51': array([
1, 0, 1, 0, 0]), 'Video52': array([0, 0, 1, 1, 0]), 'Video53': array([0, 1, 0, 0, 0]), 'Video54': array([1, 1, 1, 0, 0]), 'Video55': array([1, 1, 1, 1, 0]), 'Vi
deo56': array([1, 0, 1, 0, 1]), 'Video57': array([1, 1, 1, 0, 1]), 'Video58': array([0, 0, 0, 0, 1]), 'Video59': array([0, 0, 0, 1, 1]), 'Video60': array([1, 1,
 0, 0, 1]), 'Video61': array([0, 0, 0, 1, 1]), 'Video62': array([0, 1, 1, 1, 1]), 'Video63': array([1, 0, 1, 0, 0]), 'Video64': array([1, 0, 0, 0, 1]), 'Video65
': array([1, 0, 1, 1, 1]), 'Video66': array([1, 0, 0, 1, 1]), 'Video67': array([0, 0, 1, 0, 1]), 'Video68': array([1, 0, 0, 1, 1]), 'Video69': array([0, 1, 0, 1
, 0]), 'Video70': array([0, 0, 1, 1, 0]), 'Video71': array([1, 0, 0, 1, 1]), 'Video72': array([0, 1, 1, 0, 0]), 'Video73': array([1, 0, 0, 1, 0]), 'Video74': ar
ray([0, 1, 1, 0, 0]), 'Video75': array([1, 1, 1, 1, 0]), 'Video76': array([1, 1, 0, 1, 0]), 'Video77': array([1, 1, 1, 0, 0]), 'Video78': array([1, 0, 0, 0, 0])
, 'Video79': array([0, 0, 1, 1, 0]), 'Video80': array([1, 1, 1, 1, 0]), 'Video81': array([1, 1, 0, 0, 1]), 'Video82': array([1, 1, 1, 1, 1]), 'Video83': array([
0, 1, 1, 0, 0]), 'Video84': array([0, 0, 1, 1, 1]), 'Video85': array([1, 1, 1, 0, 1]), 'Video86': array([0, 0, 1, 0, 1]), 'Video87': array([0, 1, 0, 1, 1]), 'Vi
deo88': array([1, 1, 1, 0, 0]), 'Video89': array([0, 1, 0, 1, 1]), 'Video90': array([0, 0, 1, 0, 1]), 'Video91': array([1, 1, 1, 1, 0]), 'Video92': array([0, 1,
 1, 0, 0]), 'Video93': array([1, 0, 1, 0, 1]), 'Video94': array([0, 1, 0, 0, 1]), 'Video95': array([0, 1, 0, 1, 0]), 'Video96': array([1, 0, 1, 0, 1]), 'Video97': array([1, 1, 0, 1, 0]), 'Video98': array([1, 0, 1, 0, 0]), 'Video99': array([1, 0, 0, 1, 0]), 'Video100': array([0, 0, 1, 0, 1])}


```

### 2. 模拟用户

```shell
{'User1': array([0, 0, 0, 1, 0]), 'User2': array([0, 1, 0, 1, 1]), 'User3': array([0, 1, 0, 1, 1]), 'User4': array([0, 1, 1, 1, 1]), 'User5': array([1, 0, 1, 0, 1]), 'User6': array([0, 0, 1, 1, 1])}
```

### 3. 余弦相似度

```shell
{'Video1': [('User1', 0.0), ('User2', 0.5773502691896258), ('User3', 0.5773502691896258), ('User4', 0.5), ('User5', 0.0), ('User6', 0.0)], 'Video2': [('User1', 
0.0), ('User2', 0.408248290463863), ('User3', 0.408248290463863), ('User4', 0.35355339059327373), ('User5', 0.816496580927726), ('User6', 0.408248290463863)], '
Video3': [('User1', 0.0), ('User2', 0.5773502691896258), ('User3', 0.5773502691896258), ('User4', 0.5), ('User5', 0.5773502691896258), ('User6', 0.5773502691896
258)], 'Video4': [('User1', 0.5773502691896258), ('User2', 0.6666666666666669), ('User3', 0.6666666666666669), ('User4', 0.8660254037844388), ('User5', 0.333333
3333333334), ('User6', 0.6666666666666669)], 'Video5': [('User1', 0.5), ('User2', 0.5773502691896258), ('User3', 0.5773502691896258), ('User4', 0.75), ('User5',
 0.8660254037844388), ('User6', 0.8660254037844388)], 'Video6': [('User1', 0.4472135954999579), ('User2', 0.7745966692414834), ('User3', 0.7745966692414834), ('
User4', 0.8944271909999159), ('User5', 0.7745966692414834), ('User6', 0.7745966692414834)], 'Video7': [('User1', 0.5773502691896258), ('User2', 0.66666666666666
69), ('User3', 0.6666666666666669), ('User4', 0.8660254037844388), ('User5', 0.6666666666666669), ('User6', 1.0000000000000002)], 'Video8': [('User1', 0.0), ('U
ser2', 0.5773502691896258), ('User3', 0.5773502691896258), ('User4', 0.5), ('User5', 0.0), ('User6', 0.0)], 'Video9': [('User1', 0.5773502691896258), ('User2', 
0.6666666666666669), ('User3', 0.6666666666666669), ('User4', 0.8660254037844388), ('User5', 0.6666666666666669), ('User6', 1.0000000000000002)], 'Video10': [('
User1', 0.5), ('User2', 0.8660254037844388), ('User3', 0.8660254037844388), ('User4', 0.75), ('User5', 0.5773502691896258), ('User6', 0.5773502691896258)], 'Vid
eo11': [('User1', 0.7071067811865475), ('User2', 0.816496580927726), ('User3', 0.816496580927726), ('User4', 0.7071067811865475), ('User5', 0.0), ('User6', 0.40
8248290463863)], 'Video12': [('User1', 0.0), ('User2', 0.408248290463863), ('User3', 0.408248290463863), ('User4', 0.35355339059327373), ('User5', 0.40824829046
3863), ('User6', 0.0)], 'Video13': [('User1', 0.0), ('User2', 0.0), ('User3', 0.0), ('User4', 0.0), ('User5', 0.0), ('User6', 0.0)], 'Video14': [('User1', 0.5),
 ('User2', 0.8660254037844388), ('User3', 0.8660254037844388), ('User4', 0.75), ('User5', 0.5773502691896258), ('User6', 0.5773502691896258)], 'Video15': [('Use
r1', 0.5773502691896258), ('User2', 0.6666666666666669), ('User3', 0.6666666666666669), ('User4', 0.5773502691896258), ('User5', 0.3333333333333334), ('User6', 
0.3333333333333334)], 'Video16': [('User1', 0.0), ('User2', 0.5773502691896258), ('User3', 0.5773502691896258), ('User4', 0.75), ('User5', 0.8660254037844388), 
('User6', 0.5773502691896258)], 'Video17': [('User1', 0.7071067811865475), ('User2', 0.816496580927726), ('User3', 0.816496580927726), ('User4', 0.7071067811865
475), ('User5', 0.0), ('User6', 0.408248290463863)], 'Video18': [('User1', 0.5773502691896258), ('User2', 1.0000000000000002), ('User3', 1.0000000000000002), ('
User4', 0.8660254037844388), ('User5', 0.3333333333333334), ('User6', 0.6666666666666669)], 'Video19': [('User1', 0.4472135954999579), ('User2', 0.7745966692414
834), ('User3', 0.7745966692414834), ('User4', 0.8944271909999159), ('User5', 0.7745966692414834), ('User6', 0.7745966692414834)], 'Video20': [('User1', 0.5), (
'User2', 0.5773502691896258), ('User3', 0.5773502691896258), ('User4', 0.75), ('User5', 0.5773502691896258), ('User6', 0.5773502691896258)], 'Video21': [('User1
', 0.5), ('User2', 0.8660254037844388), ('User3', 0.8660254037844388), ('User4', 1.0), ('User5', 0.5773502691896258), ('User6', 0.8660254037844388)], 'Video22':
 [('User1', 0.5), ('User2', 0.5773502691896258), ('User3', 0.5773502691896258), ('User4', 0.75), ('User5', 0.5773502691896258), ('User6', 0.5773502691896258)], 
'Video23': [('User1', 0.5773502691896258), ('User2', 0.3333333333333334), ('User3', 0.3333333333333334), ('User4', 0.5773502691896258), ('User5', 0.666666666666
6669), ('User6', 0.6666666666666669)], 'Video24': [('User1', 0.5773502691896258), ('User2', 0.3333333333333334), ('User3', 0.3333333333333334), ('User4', 0.5773
502691896258), ('User5', 0.6666666666666669), ('User6', 0.6666666666666669)], 'Video25': [('User1', 0.0), ('User2', 0.0), ('User3', 0.0), ('User4', 0.3535533905
9327373), ('User5', 0.816496580927726), ('User6', 0.408248290463863)], 'Video26': [('User1', 0.5), ('User2', 0.8660254037844388), ('User3', 0.8660254037844388),
 ('User4', 0.75), ('User5', 0.5773502691896258), ('User6', 0.5773502691896258)], 'Video27': [('User1', 0.0), ('User2', 0.0), ('User3', 0.0), ('User4', 0.0), ('U
ser5', 0.5773502691896258), ('User6', 0.0)], 'Video28': [('User1', 0.0), ('User2', 0.0), ('User3', 0.0), ('User4', 0.0), ('User5', 0.0), ('User6', 0.0)], 'Video
29': [('User1', 0.5), ('User2', 0.5773502691896258), ('User3', 0.5773502691896258), ('User4', 0.75), ('User5', 0.8660254037844388), ('User6', 0.8660254037844388
)], 'Video30': [('User1', 0.0), ('User2', 0.5773502691896258), ('User3', 0.5773502691896258), ('User4', 0.5), ('User5', 0.5773502691896258), ('User6', 0.5773502
691896258)], 'Video31': [('User1', 0.0), ('User2', 0.0), ('User3', 0.0), ('User4', 0.0), ('User5', 0.0), ('User6', 0.0)], 'Video32': [('User1', 0.0), ('User2', 
0.3333333333333334), ('User3', 0.3333333333333334), ('User4', 0.5773502691896258), ('User5', 1.0000000000000002), ('User6', 0.6666666666666669)], 'Video33': [('
User1', 0.5773502691896258), ('User2', 0.6666666666666669), ('User3', 0.6666666666666669), ('User4', 0.8660254037844388), ('User5', 0.6666666666666669), ('User6
', 1.0000000000000002)], 'Video34': [('User1', 0.0), ('User2', 0.816496580927726), ('User3', 0.816496580927726), ('User4', 0.7071067811865475), ('User5', 0.4082
48290463863), ('User6', 0.408248290463863)], 'Video35': [('User1', 0.5773502691896258), ('User2', 0.6666666666666669), ('User3', 0.6666666666666669), ('User4', 
0.5773502691896258), ('User5', 0.6666666666666669), ('User6', 0.6666666666666669)], 'Video36': [('User1', 0.0), ('User2', 0.0), ('User3', 0.0), ('User4', 0.0), 
('User5', 0.5773502691896258), ('User6', 0.0)], 'Video37': [('User1', 0.0), ('User2', 0.0), ('User3', 0.0), ('User4', 0.5), ('User5', 0.5773502691896258), ('Use
r6', 0.5773502691896258)], 'Video38': [('User1', 0.0), ('User2', 0.816496580927726), ('User3', 0.816496580927726), ('User4', 0.7071067811865475), ('User5', 0.40
8248290463863), ('User6', 0.408248290463863)], 'Video39': [('User1', 0.0), ('User2', 0.0), ('User3', 0.0), ('User4', 0.0), ('User5', 0.0), ('User6', 0.0)], 'Vid
eo40': [('User1', 0.0), ('User2', 0.3333333333333334), ('User3', 0.3333333333333334), ('User4', 0.5773502691896258), ('User5', 0.6666666666666669), ('User6', 0.
3333333333333334)], 'Video41': [('User1', 0.0), ('User2', 0.816496580927726), ('User3', 0.816496580927726), ('User4', 0.7071067811865475), ('User5', 0.408248290
463863), ('User6', 0.408248290463863)], 'Video42': [('User1', 0.5773502691896258), ('User2', 0.6666666666666669), ('User3', 0.6666666666666669), ('User4', 0.866
0254037844388), ('User5', 0.3333333333333334), ('User6', 0.6666666666666669)], 'Video43': [('User1', 0.0), ('User2', 0.0), ('User3', 0.0), ('User4', 0.0), ('Use
r5', 0.0), ('User6', 0.0)], 'Video44': [('User1', 0.7071067811865475), ('User2', 0.816496580927726), ('User3', 0.816496580927726), ('User4', 0.7071067811865475)
, ('User5', 0.0), ('User6', 0.408248290463863)], 'Video45': [('User1', 0.5773502691896258), ('User2', 0.6666666666666669), ('User3', 0.6666666666666669), ('User
4', 0.8660254037844388), ('User5', 0.6666666666666669), ('User6', 1.0000000000000002)], 'Video46': [('User1', 0.0), ('User2', 0.0), ('User3', 0.0), ('User4', 0.
35355339059327373), ('User5', 0.816496580927726), ('User6', 0.408248290463863)], 'Video47': [('User1', 0.4472135954999579), ('User2', 0.7745966692414834), ('Use
r3', 0.7745966692414834), ('User4', 0.8944271909999159), ('User5', 0.7745966692414834), ('User6', 0.7745966692414834)], 'Video48': [('User1', 0.0), ('User2', 0.
5773502691896258), ('User3', 0.5773502691896258), ('User4', 0.75), ('User5', 0.8660254037844388), ('User6', 0.5773502691896258)], 'Video49': [('User1', 0.0), ('
User2', 0.0), ('User3', 0.0), ('User4', 0.35355339059327373), ('User5', 0.816496580927726), ('User6', 0.408248290463863)], 'Video50': [('User1', 0.0), ('User2',
 0.0), ('User3', 0.0), ('User4', 0.0), ('User5', 0.5773502691896258), ('User6', 0.0)], 'Video51': [('User1', 0.0), ('User2', 0.0), ('User3', 0.0), ('User4', 0.3
5355339059327373), ('User5', 0.816496580927726), ('User6', 0.408248290463863)], 'Video52': [('User1', 0.7071067811865475), ('User2', 0.408248290463863), ('User3
', 0.408248290463863), ('User4', 0.7071067811865475), ('User5', 0.408248290463863), ('User6', 0.816496580927726)], 'Video53': [('User1', 0.0), ('User2', 0.57735
02691896258), ('User3', 0.5773502691896258), ('User4', 0.5), ('User5', 0.0), ('User6', 0.0)], 'Video54': [('User1', 0.0), ('User2', 0.3333333333333334), ('User3
', 0.3333333333333334), ('User4', 0.5773502691896258), ('User5', 0.6666666666666669), ('User6', 0.3333333333333334)], 'Video55': [('User1', 0.5), ('User2', 0.57
73502691896258), ('User3', 0.5773502691896258), ('User4', 0.75), ('User5', 0.5773502691896258), ('User6', 0.5773502691896258)], 'Video56': [('User1', 0.0), ('Us
er2', 0.3333333333333334), ('User3', 0.3333333333333334), ('User4', 0.5773502691896258), ('User5', 1.0000000000000002), ('User6', 0.6666666666666669)], 'Video57
': [('User1', 0.0), ('User2', 0.5773502691896258), ('User3', 0.5773502691896258), ('User4', 0.75), ('User5', 0.8660254037844388), ('User6', 0.5773502691896258)]
, 'Video58': [('User1', 0.0), ('User2', 0.5773502691896258), ('User3', 0.5773502691896258), ('User4', 0.5), ('User5', 0.5773502691896258), ('User6', 0.577350269
1896258)], 'Video59': [('User1', 0.7071067811865475), ('User2', 0.816496580927726), ('User3', 0.816496580927726), ('User4', 0.7071067811865475), ('User5', 0.408
248290463863), ('User6', 0.816496580927726)], 'Video60': [('User1', 0.0), ('User2', 0.6666666666666669), ('User3', 0.6666666666666669), ('User4', 0.577350269189
6258), ('User5', 0.6666666666666669), ('User6', 0.3333333333333334)], 'Video61': [('User1', 0.7071067811865475), ('User2', 0.816496580927726), ('User3', 0.81649
6580927726), ('User4', 0.7071067811865475), ('User5', 0.408248290463863), ('User6', 0.816496580927726)], 'Video62': [('User1', 0.5), ('User2', 0.866025403784438
8), ('User3', 0.8660254037844388), ('User4', 1.0), ('User5', 0.5773502691896258), ('User6', 0.8660254037844388)], 'Video63': [('User1', 0.0), ('User2', 0.0), ('
User3', 0.0), ('User4', 0.35355339059327373), ('User5', 0.816496580927726), ('User6', 0.408248290463863)], 'Video64': [('User1', 0.0), ('User2', 0.4082482904638
63), ('User3', 0.408248290463863), ('User4', 0.35355339059327373), ('User5', 0.816496580927726), ('User6', 0.408248290463863)], 'Video65': [('User1', 0.5), ('Us
er2', 0.5773502691896258), ('User3', 0.5773502691896258), ('User4', 0.75), ('User5', 0.8660254037844388), ('User6', 0.8660254037844388)], 'Video66': [('User1', 
0.5773502691896258), ('User2', 0.6666666666666669), ('User3', 0.6666666666666669), ('User4', 0.5773502691896258), ('User5', 0.6666666666666669), ('User6', 0.666
6666666666669)], 'Video67': [('User1', 0.0), ('User2', 0.408248290463863), ('User3', 0.408248290463863), ('User4', 0.7071067811865475), ('User5', 0.816496580927
726), ('User6', 0.816496580927726)], 'Video68': [('User1', 0.5773502691896258), ('User2', 0.6666666666666669), ('User3', 0.6666666666666669), ('User4', 0.577350
2691896258), ('User5', 0.6666666666666669), ('User6', 0.6666666666666669)], 'Video69': [('User1', 0.7071067811865475), ('User2', 0.816496580927726), ('User3', 0
.816496580927726), ('User4', 0.7071067811865475), ('User5', 0.0), ('User6', 0.408248290463863)], 'Video70': [('User1', 0.7071067811865475), ('User2', 0.40824829
0463863), ('User3', 0.408248290463863), ('User4', 0.7071067811865475), ('User5', 0.408248290463863), ('User6', 0.816496580927726)], 'Video71': [('User1', 0.5773
502691896258), ('User2', 0.6666666666666669), ('User3', 0.6666666666666669), ('User4', 0.5773502691896258), ('User5', 0.6666666666666669), ('User6', 0.666666666
6666669)], 'Video72': [('User1', 0.0), ('User2', 0.408248290463863), ('User3', 0.408248290463863), ('User4', 0.7071067811865475), ('User5', 0.408248290463863), 
('User6', 0.408248290463863)], 'Video73': [('User1', 0.7071067811865475), ('User2', 0.408248290463863), ('User3', 0.408248290463863), ('User4', 0.35355339059327
373), ('User5', 0.408248290463863), ('User6', 0.408248290463863)], 'Video74': [('User1', 0.0), ('User2', 0.408248290463863), ('User3', 0.408248290463863), ('Use
r4', 0.7071067811865475), ('User5', 0.408248290463863), ('User6', 0.408248290463863)], 'Video75': [('User1', 0.5), ('User2', 0.5773502691896258), ('User3', 0.57
73502691896258), ('User4', 0.75), ('User5', 0.5773502691896258), ('User6', 0.5773502691896258)], 'Video76': [('User1', 0.5773502691896258), ('User2', 0.66666666
66666669), ('User3', 0.6666666666666669), ('User4', 0.5773502691896258), ('User5', 0.3333333333333334), ('User6', 0.3333333333333334)], 'Video77': [('User1', 0.
0), ('User2', 0.3333333333333334), ('User3', 0.3333333333333334), ('User4', 0.5773502691896258), ('User5', 0.6666666666666669), ('User6', 0.3333333333333334)], 
'Video78': [('User1', 0.0), ('User2', 0.0), ('User3', 0.0), ('User4', 0.0), ('User5', 0.5773502691896258), ('User6', 0.0)], 'Video79': [('User1', 0.707106781186
5475), ('User2', 0.408248290463863), ('User3', 0.408248290463863), ('User4', 0.7071067811865475), ('User5', 0.408248290463863), ('User6', 0.816496580927726)], '
Video80': [('User1', 0.5), ('User2', 0.5773502691896258), ('User3', 0.5773502691896258), ('User4', 0.75), ('User5', 0.5773502691896258), ('User6', 0.57735026918
96258)], 'Video81': [('User1', 0.0), ('User2', 0.6666666666666669), ('User3', 0.6666666666666669), ('User4', 0.5773502691896258), ('User5', 0.6666666666666669),
 ('User6', 0.3333333333333334)], 'Video82': [('User1', 0.4472135954999579), ('User2', 0.7745966692414834), ('User3', 0.7745966692414834), ('User4', 0.8944271909
999159), ('User5', 0.7745966692414834), ('User6', 0.7745966692414834)], 'Video83': [('User1', 0.0), ('User2', 0.408248290463863), ('User3', 0.408248290463863), 
('User4', 0.7071067811865475), ('User5', 0.408248290463863), ('User6', 0.408248290463863)], 'Video84': [('User1', 0.5773502691896258), ('User2', 0.6666666666666
669), ('User3', 0.6666666666666669), ('User4', 0.8660254037844388), ('User5', 0.6666666666666669), ('User6', 1.0000000000000002)], 'Video85': [('User1', 0.0), (
'User2', 0.5773502691896258), ('User3', 0.5773502691896258), ('User4', 0.75), ('User5', 0.8660254037844388), ('User6', 0.5773502691896258)], 'Video86': [('User1
', 0.0), ('User2', 0.408248290463863), ('User3', 0.408248290463863), ('User4', 0.7071067811865475), ('User5', 0.816496580927726), ('User6', 0.816496580927726)],
 'Video87': [('User1', 0.5773502691896258), ('User2', 1.0000000000000002), ('User3', 1.0000000000000002), ('User4', 0.8660254037844388), ('User5', 0.33333333333
33334), ('User6', 0.6666666666666669)], 'Video88': [('User1', 0.0), ('User2', 0.3333333333333334), ('User3', 0.3333333333333334), ('User4', 0.5773502691896258),
 ('User5', 0.6666666666666669), ('User6', 0.3333333333333334)], 'Video89': [('User1', 0.5773502691896258), ('User2', 1.0000000000000002), ('User3', 1.0000000000
000002), ('User4', 0.8660254037844388), ('User5', 0.3333333333333334), ('User6', 0.6666666666666669)], 'Video90': [('User1', 0.0), ('User2', 0.408248290463863),
 ('User3', 0.408248290463863), ('User4', 0.7071067811865475), ('User5', 0.816496580927726), ('User6', 0.816496580927726)], 'Video91': [('User1', 0.5), ('User2',
 0.5773502691896258), ('User3', 0.5773502691896258), ('User4', 0.75), ('User5', 0.5773502691896258), ('User6', 0.5773502691896258)], 'Video92': [('User1', 0.0),
 ('User2', 0.408248290463863), ('User3', 0.408248290463863), ('User4', 0.7071067811865475), ('User5', 0.408248290463863), ('User6', 0.408248290463863)], 'Video9
3': [('User1', 0.0), ('User2', 0.3333333333333334), ('User3', 0.3333333333333334), ('User4', 0.5773502691896258), ('User5', 1.0000000000000002), ('User6', 0.666
6666666666669)], 'Video94': [('User1', 0.0), ('User2', 0.816496580927726), ('User3', 0.816496580927726), ('User4', 0.7071067811865475), ('User5', 0.408248290463
863), ('User6', 0.408248290463863)], 'Video95': [('User1', 0.7071067811865475), ('User2', 0.816496580927726), ('User3', 0.816496580927726), ('User4', 0.70710678
11865475), ('User5', 0.0), ('User6', 0.408248290463863)], 'Video96': [('User1', 0.0), ('User2', 0.3333333333333334), ('User3', 0.3333333333333334), ('User4', 0.
5773502691896258), ('User5', 1.0000000000000002), ('User6', 0.6666666666666669)], 'Video97': [('User1', 0.5773502691896258), ('User2', 0.6666666666666669), ('Us
er3', 0.6666666666666669), ('User4', 0.5773502691896258), ('User5', 0.3333333333333334), ('User6', 0.3333333333333334)], 'Video98': [('User1', 0.0), ('User2', 0
.0), ('User3', 0.0), ('User4', 0.35355339059327373), ('User5', 0.816496580927726), ('User6', 0.408248290463863)], 'Video99': [('User1', 0.7071067811865475), ('U
ser2', 0.408248290463863), ('User3', 0.408248290463863), ('User4', 0.35355339059327373), ('User5', 0.408248290463863), ('User6', 0.408248290463863)], 'Video100'
: [('User1', 0.0), ('User2', 0.408248290463863), ('User3', 0.408248290463863), ('User4', 0.7071067811865475), ('User5', 0.816496580927726), ('User6', 0.816496580927726)]}

```

### 4. 结果

```shell
Video1 的推荐：
   User2 - 相似度: 0.5773502691896258
   User3 - 相似度: 0.5773502691896258
   User4 - 相似度: 0.5
   User1 - 相似度: 0.0
   User5 - 相似度: 0.0
   User6 - 相似度: 0.0
Video2 的推荐：
   User5 - 相似度: 0.816496580927726
   User2 - 相似度: 0.408248290463863
   User3 - 相似度: 0.408248290463863
   User6 - 相似度: 0.408248290463863
   User4 - 相似度: 0.35355339059327373
   User1 - 相似度: 0.0
Video3 的推荐：
   User2 - 相似度: 0.5773502691896258
   User3 - 相似度: 0.5773502691896258
   User5 - 相似度: 0.5773502691896258
   User6 - 相似度: 0.5773502691896258
   User4 - 相似度: 0.5
   User1 - 相似度: 0.0
Video4 的推荐：
   User4 - 相似度: 0.8660254037844388
   User2 - 相似度: 0.6666666666666669
   User3 - 相似度: 0.6666666666666669
   User6 - 相似度: 0.6666666666666669
   User1 - 相似度: 0.5773502691896258
   User5 - 相似度: 0.3333333333333334
Video5 的推荐：
   User5 - 相似度: 0.8660254037844388
   User6 - 相似度: 0.8660254037844388
   User4 - 相似度: 0.75
   User2 - 相似度: 0.5773502691896258
   User3 - 相似度: 0.5773502691896258
   User1 - 相似度: 0.5
Video6 的推荐：
   User4 - 相似度: 0.8944271909999159
   User2 - 相似度: 0.7745966692414834
   User3 - 相似度: 0.7745966692414834
   User5 - 相似度: 0.7745966692414834
   User6 - 相似度: 0.7745966692414834
   User1 - 相似度: 0.4472135954999579
Video7 的推荐：
   User6 - 相似度: 1.0000000000000002
   User4 - 相似度: 0.8660254037844388
   User2 - 相似度: 0.6666666666666669
   User3 - 相似度: 0.6666666666666669
   User5 - 相似度: 0.6666666666666669
   User1 - 相似度: 0.5773502691896258
Video8 的推荐：
   User2 - 相似度: 0.5773502691896258
   User3 - 相似度: 0.5773502691896258
   User4 - 相似度: 0.5
   User1 - 相似度: 0.0
   User5 - 相似度: 0.0
   User6 - 相似度: 0.0
Video9 的推荐：
   User6 - 相似度: 1.0000000000000002
   User4 - 相似度: 0.8660254037844388
   User2 - 相似度: 0.6666666666666669
   User3 - 相似度: 0.6666666666666669
   User5 - 相似度: 0.6666666666666669
   User1 - 相似度: 0.5773502691896258
Video10 的推荐：
   User2 - 相似度: 0.8660254037844388
   User3 - 相似度: 0.8660254037844388
   User4 - 相似度: 0.75
   User5 - 相似度: 0.5773502691896258
   User6 - 相似度: 0.5773502691896258
   User1 - 相似度: 0.5
Video11 的推荐：
   User2 - 相似度: 0.816496580927726
   User3 - 相似度: 0.816496580927726
   User1 - 相似度: 0.7071067811865475
   User4 - 相似度: 0.7071067811865475
   User6 - 相似度: 0.408248290463863
   User5 - 相似度: 0.0
Video12 的推荐：
   User2 - 相似度: 0.408248290463863
   User3 - 相似度: 0.408248290463863
   User5 - 相似度: 0.408248290463863
   User4 - 相似度: 0.35355339059327373
   User1 - 相似度: 0.0
   User6 - 相似度: 0.0
Video13 的推荐：
   User1 - 相似度: 0.0
   User2 - 相似度: 0.0
   User3 - 相似度: 0.0
   User4 - 相似度: 0.0
   User5 - 相似度: 0.0
   User6 - 相似度: 0.0
Video14 的推荐：
   User2 - 相似度: 0.8660254037844388
   User3 - 相似度: 0.8660254037844388
   User4 - 相似度: 0.75
   User5 - 相似度: 0.5773502691896258
   User6 - 相似度: 0.5773502691896258
   User1 - 相似度: 0.5
Video15 的推荐：
   User2 - 相似度: 0.6666666666666669
   User3 - 相似度: 0.6666666666666669
   User1 - 相似度: 0.5773502691896258
   User4 - 相似度: 0.5773502691896258
   User5 - 相似度: 0.3333333333333334
   User6 - 相似度: 0.3333333333333334
Video16 的推荐：
   User5 - 相似度: 0.8660254037844388
   User4 - 相似度: 0.75
   User2 - 相似度: 0.5773502691896258
   User3 - 相似度: 0.5773502691896258
   User6 - 相似度: 0.5773502691896258
   User1 - 相似度: 0.0
Video17 的推荐：
   User2 - 相似度: 0.816496580927726
   User3 - 相似度: 0.816496580927726
   User1 - 相似度: 0.7071067811865475
   User4 - 相似度: 0.7071067811865475
   User6 - 相似度: 0.408248290463863
   User5 - 相似度: 0.0
Video18 的推荐：
   User2 - 相似度: 1.0000000000000002
   User3 - 相似度: 1.0000000000000002
   User4 - 相似度: 0.8660254037844388
   User6 - 相似度: 0.6666666666666669
   User1 - 相似度: 0.5773502691896258
   User5 - 相似度: 0.3333333333333334
Video19 的推荐：
   User4 - 相似度: 0.8944271909999159
   User2 - 相似度: 0.7745966692414834
   User3 - 相似度: 0.7745966692414834
   User5 - 相似度: 0.7745966692414834
   User6 - 相似度: 0.7745966692414834
   User1 - 相似度: 0.4472135954999579
Video20 的推荐：
   User4 - 相似度: 0.75
   User2 - 相似度: 0.5773502691896258
   User3 - 相似度: 0.5773502691896258
   User5 - 相似度: 0.5773502691896258
   User6 - 相似度: 0.5773502691896258
   User1 - 相似度: 0.5
Video21 的推荐：
   User4 - 相似度: 1.0
   User2 - 相似度: 0.8660254037844388
   User3 - 相似度: 0.8660254037844388
   User6 - 相似度: 0.8660254037844388
   User5 - 相似度: 0.5773502691896258
   User1 - 相似度: 0.5
Video22 的推荐：
   User4 - 相似度: 0.75
   User2 - 相似度: 0.5773502691896258
   User3 - 相似度: 0.5773502691896258
   User5 - 相似度: 0.5773502691896258
   User6 - 相似度: 0.5773502691896258
   User1 - 相似度: 0.5
Video23 的推荐：
   User5 - 相似度: 0.6666666666666669
   User6 - 相似度: 0.6666666666666669
   User1 - 相似度: 0.5773502691896258
   User4 - 相似度: 0.5773502691896258
   User2 - 相似度: 0.3333333333333334
   User3 - 相似度: 0.3333333333333334
Video24 的推荐：
   User5 - 相似度: 0.6666666666666669
   User6 - 相似度: 0.6666666666666669
   User1 - 相似度: 0.5773502691896258
   User4 - 相似度: 0.5773502691896258
   User2 - 相似度: 0.3333333333333334
   User3 - 相似度: 0.3333333333333334
Video25 的推荐：
   User5 - 相似度: 0.816496580927726
   User6 - 相似度: 0.408248290463863
   User4 - 相似度: 0.35355339059327373
   User1 - 相似度: 0.0
   User2 - 相似度: 0.0
   User3 - 相似度: 0.0
Video26 的推荐：
   User2 - 相似度: 0.8660254037844388
   User3 - 相似度: 0.8660254037844388
   User4 - 相似度: 0.75
   User5 - 相似度: 0.5773502691896258
   User6 - 相似度: 0.5773502691896258
   User1 - 相似度: 0.5
Video27 的推荐：
   User5 - 相似度: 0.5773502691896258
   User1 - 相似度: 0.0
   User2 - 相似度: 0.0
   User3 - 相似度: 0.0
   User4 - 相似度: 0.0
   User6 - 相似度: 0.0
Video28 的推荐：
   User1 - 相似度: 0.0
   User2 - 相似度: 0.0
   User3 - 相似度: 0.0
   User4 - 相似度: 0.0
   User5 - 相似度: 0.0
   User6 - 相似度: 0.0
Video29 的推荐：
   User5 - 相似度: 0.8660254037844388
   User6 - 相似度: 0.8660254037844388
   User4 - 相似度: 0.75
   User2 - 相似度: 0.5773502691896258
   User3 - 相似度: 0.5773502691896258
   User1 - 相似度: 0.5
Video30 的推荐：
   User2 - 相似度: 0.5773502691896258
   User3 - 相似度: 0.5773502691896258
   User5 - 相似度: 0.5773502691896258
   User6 - 相似度: 0.5773502691896258
   User4 - 相似度: 0.5
   User1 - 相似度: 0.0
Video31 的推荐：
   User1 - 相似度: 0.0
   User2 - 相似度: 0.0
   User3 - 相似度: 0.0
   User4 - 相似度: 0.0
   User5 - 相似度: 0.0
   User6 - 相似度: 0.0
Video32 的推荐：
   User5 - 相似度: 1.0000000000000002
   User6 - 相似度: 0.6666666666666669
   User4 - 相似度: 0.5773502691896258
   User2 - 相似度: 0.3333333333333334
   User3 - 相似度: 0.3333333333333334
   User1 - 相似度: 0.0
Video33 的推荐：
   User6 - 相似度: 1.0000000000000002
   User4 - 相似度: 0.8660254037844388
   User2 - 相似度: 0.6666666666666669
   User3 - 相似度: 0.6666666666666669
   User5 - 相似度: 0.6666666666666669
   User1 - 相似度: 0.5773502691896258
Video34 的推荐：
   User2 - 相似度: 0.816496580927726
   User3 - 相似度: 0.816496580927726
   User4 - 相似度: 0.7071067811865475
   User5 - 相似度: 0.408248290463863
   User6 - 相似度: 0.408248290463863
   User1 - 相似度: 0.0
Video35 的推荐：
   User2 - 相似度: 0.6666666666666669
   User3 - 相似度: 0.6666666666666669
   User5 - 相似度: 0.6666666666666669
   User6 - 相似度: 0.6666666666666669
   User1 - 相似度: 0.5773502691896258
   User4 - 相似度: 0.5773502691896258
Video36 的推荐：
   User5 - 相似度: 0.5773502691896258
   User1 - 相似度: 0.0
   User2 - 相似度: 0.0
   User3 - 相似度: 0.0
   User4 - 相似度: 0.0
   User6 - 相似度: 0.0
Video37 的推荐：
   User5 - 相似度: 0.5773502691896258
   User6 - 相似度: 0.5773502691896258
   User4 - 相似度: 0.5
   User1 - 相似度: 0.0
   User2 - 相似度: 0.0
   User3 - 相似度: 0.0
Video38 的推荐：
   User2 - 相似度: 0.816496580927726
   User3 - 相似度: 0.816496580927726
   User4 - 相似度: 0.7071067811865475
   User5 - 相似度: 0.408248290463863
   User6 - 相似度: 0.408248290463863
   User1 - 相似度: 0.0
Video39 的推荐：
   User1 - 相似度: 0.0
   User2 - 相似度: 0.0
   User3 - 相似度: 0.0
   User4 - 相似度: 0.0
   User5 - 相似度: 0.0
   User6 - 相似度: 0.0
Video40 的推荐：
   User5 - 相似度: 0.6666666666666669
   User4 - 相似度: 0.5773502691896258
   User2 - 相似度: 0.3333333333333334
   User3 - 相似度: 0.3333333333333334
   User6 - 相似度: 0.3333333333333334
   User1 - 相似度: 0.0
Video41 的推荐：
   User2 - 相似度: 0.816496580927726
   User3 - 相似度: 0.816496580927726
   User4 - 相似度: 0.7071067811865475
   User5 - 相似度: 0.408248290463863
   User6 - 相似度: 0.408248290463863
   User1 - 相似度: 0.0
Video42 的推荐：
   User4 - 相似度: 0.8660254037844388
   User2 - 相似度: 0.6666666666666669
   User3 - 相似度: 0.6666666666666669
   User6 - 相似度: 0.6666666666666669
   User1 - 相似度: 0.5773502691896258
   User5 - 相似度: 0.3333333333333334
Video43 的推荐：
   User1 - 相似度: 0.0
   User2 - 相似度: 0.0
   User3 - 相似度: 0.0
   User4 - 相似度: 0.0
   User5 - 相似度: 0.0
   User6 - 相似度: 0.0
Video44 的推荐：
   User2 - 相似度: 0.816496580927726
   User3 - 相似度: 0.816496580927726
   User1 - 相似度: 0.7071067811865475
   User4 - 相似度: 0.7071067811865475
   User6 - 相似度: 0.408248290463863
   User5 - 相似度: 0.0
Video45 的推荐：
   User6 - 相似度: 1.0000000000000002
   User4 - 相似度: 0.8660254037844388
   User2 - 相似度: 0.6666666666666669
   User3 - 相似度: 0.6666666666666669
   User5 - 相似度: 0.6666666666666669
   User1 - 相似度: 0.5773502691896258
Video46 的推荐：
   User5 - 相似度: 0.816496580927726
   User6 - 相似度: 0.408248290463863
   User4 - 相似度: 0.35355339059327373
   User1 - 相似度: 0.0
   User2 - 相似度: 0.0
   User3 - 相似度: 0.0
Video47 的推荐：
   User4 - 相似度: 0.8944271909999159
   User2 - 相似度: 0.7745966692414834
   User3 - 相似度: 0.7745966692414834
   User5 - 相似度: 0.7745966692414834
   User6 - 相似度: 0.7745966692414834
   User1 - 相似度: 0.4472135954999579
Video48 的推荐：
   User5 - 相似度: 0.8660254037844388
   User4 - 相似度: 0.75
   User2 - 相似度: 0.5773502691896258
   User3 - 相似度: 0.5773502691896258
   User6 - 相似度: 0.5773502691896258
   User1 - 相似度: 0.0
Video49 的推荐：
   User5 - 相似度: 0.816496580927726
   User6 - 相似度: 0.408248290463863
   User4 - 相似度: 0.35355339059327373
   User1 - 相似度: 0.0
   User2 - 相似度: 0.0
   User3 - 相似度: 0.0
Video50 的推荐：
   User5 - 相似度: 0.5773502691896258
   User1 - 相似度: 0.0
   User2 - 相似度: 0.0
   User3 - 相似度: 0.0
   User4 - 相似度: 0.0
   User6 - 相似度: 0.0
Video51 的推荐：
   User5 - 相似度: 0.816496580927726
   User6 - 相似度: 0.408248290463863
   User4 - 相似度: 0.35355339059327373
   User1 - 相似度: 0.0
   User2 - 相似度: 0.0
   User3 - 相似度: 0.0
Video52 的推荐：
   User6 - 相似度: 0.816496580927726
   User1 - 相似度: 0.7071067811865475
   User4 - 相似度: 0.7071067811865475
   User2 - 相似度: 0.408248290463863
   User3 - 相似度: 0.408248290463863
   User5 - 相似度: 0.408248290463863
Video53 的推荐：
   User2 - 相似度: 0.5773502691896258
   User3 - 相似度: 0.5773502691896258
   User4 - 相似度: 0.5
   User1 - 相似度: 0.0
   User5 - 相似度: 0.0
   User6 - 相似度: 0.0
Video54 的推荐：
   User5 - 相似度: 0.6666666666666669
   User4 - 相似度: 0.5773502691896258
   User2 - 相似度: 0.3333333333333334
   User3 - 相似度: 0.3333333333333334
   User6 - 相似度: 0.3333333333333334
   User1 - 相似度: 0.0
Video55 的推荐：
   User4 - 相似度: 0.75
   User2 - 相似度: 0.5773502691896258
   User3 - 相似度: 0.5773502691896258
   User5 - 相似度: 0.5773502691896258
   User6 - 相似度: 0.5773502691896258
   User1 - 相似度: 0.5
Video56 的推荐：
   User5 - 相似度: 1.0000000000000002
   User6 - 相似度: 0.6666666666666669
   User4 - 相似度: 0.5773502691896258
   User2 - 相似度: 0.3333333333333334
   User3 - 相似度: 0.3333333333333334
   User1 - 相似度: 0.0
Video57 的推荐：
   User5 - 相似度: 0.8660254037844388
   User4 - 相似度: 0.75
   User2 - 相似度: 0.5773502691896258
   User3 - 相似度: 0.5773502691896258
   User6 - 相似度: 0.5773502691896258
   User1 - 相似度: 0.0
Video58 的推荐：
   User2 - 相似度: 0.5773502691896258
   User3 - 相似度: 0.5773502691896258
   User5 - 相似度: 0.5773502691896258
   User6 - 相似度: 0.5773502691896258
   User4 - 相似度: 0.5
   User1 - 相似度: 0.0
Video59 的推荐：
   User2 - 相似度: 0.816496580927726
   User3 - 相似度: 0.816496580927726
   User6 - 相似度: 0.816496580927726
   User1 - 相似度: 0.7071067811865475
   User4 - 相似度: 0.7071067811865475
   User5 - 相似度: 0.408248290463863
Video60 的推荐：
   User2 - 相似度: 0.6666666666666669
   User3 - 相似度: 0.6666666666666669
   User5 - 相似度: 0.6666666666666669
   User4 - 相似度: 0.5773502691896258
   User6 - 相似度: 0.3333333333333334
   User1 - 相似度: 0.0
Video61 的推荐：
   User2 - 相似度: 0.816496580927726
   User3 - 相似度: 0.816496580927726
   User6 - 相似度: 0.816496580927726
   User1 - 相似度: 0.7071067811865475
   User4 - 相似度: 0.7071067811865475
   User5 - 相似度: 0.408248290463863
Video62 的推荐：
   User4 - 相似度: 1.0
   User2 - 相似度: 0.8660254037844388
   User3 - 相似度: 0.8660254037844388
   User6 - 相似度: 0.8660254037844388
   User5 - 相似度: 0.5773502691896258
   User1 - 相似度: 0.5
Video63 的推荐：
   User5 - 相似度: 0.816496580927726
   User6 - 相似度: 0.408248290463863
   User4 - 相似度: 0.35355339059327373
   User1 - 相似度: 0.0
   User2 - 相似度: 0.0
   User3 - 相似度: 0.0
Video64 的推荐：
   User5 - 相似度: 0.816496580927726
   User2 - 相似度: 0.408248290463863
   User3 - 相似度: 0.408248290463863
   User6 - 相似度: 0.408248290463863
   User4 - 相似度: 0.35355339059327373
   User1 - 相似度: 0.0
Video65 的推荐：
   User5 - 相似度: 0.8660254037844388
   User6 - 相似度: 0.8660254037844388
   User4 - 相似度: 0.75
   User2 - 相似度: 0.5773502691896258
   User3 - 相似度: 0.5773502691896258
   User1 - 相似度: 0.5
Video66 的推荐：
   User2 - 相似度: 0.6666666666666669
   User3 - 相似度: 0.6666666666666669
   User5 - 相似度: 0.6666666666666669
   User6 - 相似度: 0.6666666666666669
   User1 - 相似度: 0.5773502691896258
   User4 - 相似度: 0.5773502691896258
Video67 的推荐：
   User5 - 相似度: 0.816496580927726
   User6 - 相似度: 0.816496580927726
   User4 - 相似度: 0.7071067811865475
   User2 - 相似度: 0.408248290463863
   User3 - 相似度: 0.408248290463863
   User1 - 相似度: 0.0
Video68 的推荐：
   User2 - 相似度: 0.6666666666666669
   User3 - 相似度: 0.6666666666666669
   User5 - 相似度: 0.6666666666666669
   User6 - 相似度: 0.6666666666666669
   User1 - 相似度: 0.5773502691896258
   User4 - 相似度: 0.5773502691896258
Video69 的推荐：
   User2 - 相似度: 0.816496580927726
   User3 - 相似度: 0.816496580927726
   User1 - 相似度: 0.7071067811865475
   User4 - 相似度: 0.7071067811865475
   User6 - 相似度: 0.408248290463863
   User5 - 相似度: 0.0
Video70 的推荐：
   User6 - 相似度: 0.816496580927726
   User1 - 相似度: 0.7071067811865475
   User4 - 相似度: 0.7071067811865475
   User2 - 相似度: 0.408248290463863
   User3 - 相似度: 0.408248290463863
   User5 - 相似度: 0.408248290463863
Video71 的推荐：
   User2 - 相似度: 0.6666666666666669
   User3 - 相似度: 0.6666666666666669
   User5 - 相似度: 0.6666666666666669
   User6 - 相似度: 0.6666666666666669
   User1 - 相似度: 0.5773502691896258
   User4 - 相似度: 0.5773502691896258
Video72 的推荐：
   User4 - 相似度: 0.7071067811865475
   User2 - 相似度: 0.408248290463863
   User3 - 相似度: 0.408248290463863
   User5 - 相似度: 0.408248290463863
   User6 - 相似度: 0.408248290463863
   User1 - 相似度: 0.0
Video73 的推荐：
   User1 - 相似度: 0.7071067811865475
   User2 - 相似度: 0.408248290463863
   User3 - 相似度: 0.408248290463863
   User5 - 相似度: 0.408248290463863
   User6 - 相似度: 0.408248290463863
   User4 - 相似度: 0.35355339059327373
Video74 的推荐：
   User4 - 相似度: 0.7071067811865475
   User2 - 相似度: 0.408248290463863
   User3 - 相似度: 0.408248290463863
   User5 - 相似度: 0.408248290463863
   User6 - 相似度: 0.408248290463863
   User1 - 相似度: 0.0
Video75 的推荐：
   User4 - 相似度: 0.75
   User2 - 相似度: 0.5773502691896258
   User3 - 相似度: 0.5773502691896258
   User5 - 相似度: 0.5773502691896258
   User6 - 相似度: 0.5773502691896258
   User1 - 相似度: 0.5
Video76 的推荐：
   User2 - 相似度: 0.6666666666666669
   User3 - 相似度: 0.6666666666666669
   User1 - 相似度: 0.5773502691896258
   User4 - 相似度: 0.5773502691896258
   User5 - 相似度: 0.3333333333333334
   User6 - 相似度: 0.3333333333333334
Video77 的推荐：
   User5 - 相似度: 0.6666666666666669
   User4 - 相似度: 0.5773502691896258
   User2 - 相似度: 0.3333333333333334
   User3 - 相似度: 0.3333333333333334
   User6 - 相似度: 0.3333333333333334
   User1 - 相似度: 0.0
Video78 的推荐：
   User5 - 相似度: 0.5773502691896258
   User1 - 相似度: 0.0
   User2 - 相似度: 0.0
   User3 - 相似度: 0.0
   User4 - 相似度: 0.0
   User6 - 相似度: 0.0
Video79 的推荐：
   User6 - 相似度: 0.816496580927726
   User1 - 相似度: 0.7071067811865475
   User4 - 相似度: 0.7071067811865475
   User2 - 相似度: 0.408248290463863
   User3 - 相似度: 0.408248290463863
   User5 - 相似度: 0.408248290463863
Video80 的推荐：
   User4 - 相似度: 0.75
   User2 - 相似度: 0.5773502691896258
   User3 - 相似度: 0.5773502691896258
   User5 - 相似度: 0.5773502691896258
   User6 - 相似度: 0.5773502691896258
   User1 - 相似度: 0.5
Video81 的推荐：
   User2 - 相似度: 0.6666666666666669
   User3 - 相似度: 0.6666666666666669
   User5 - 相似度: 0.6666666666666669
   User4 - 相似度: 0.5773502691896258
   User6 - 相似度: 0.3333333333333334
   User1 - 相似度: 0.0
Video82 的推荐：
   User4 - 相似度: 0.8944271909999159
   User2 - 相似度: 0.7745966692414834
   User3 - 相似度: 0.7745966692414834
   User5 - 相似度: 0.7745966692414834
   User6 - 相似度: 0.7745966692414834
   User1 - 相似度: 0.4472135954999579
Video83 的推荐：
   User4 - 相似度: 0.7071067811865475
   User2 - 相似度: 0.408248290463863
   User3 - 相似度: 0.408248290463863
   User5 - 相似度: 0.408248290463863
   User6 - 相似度: 0.408248290463863
   User1 - 相似度: 0.0
Video84 的推荐：
   User6 - 相似度: 1.0000000000000002
   User4 - 相似度: 0.8660254037844388
   User2 - 相似度: 0.6666666666666669
   User3 - 相似度: 0.6666666666666669
   User5 - 相似度: 0.6666666666666669
   User1 - 相似度: 0.5773502691896258
Video85 的推荐：
   User5 - 相似度: 0.8660254037844388
   User4 - 相似度: 0.75
   User2 - 相似度: 0.5773502691896258
   User3 - 相似度: 0.5773502691896258
   User6 - 相似度: 0.5773502691896258
   User1 - 相似度: 0.0
Video86 的推荐：
   User5 - 相似度: 0.816496580927726
   User6 - 相似度: 0.816496580927726
   User4 - 相似度: 0.7071067811865475
   User2 - 相似度: 0.408248290463863
   User3 - 相似度: 0.408248290463863
   User1 - 相似度: 0.0
Video87 的推荐：
   User2 - 相似度: 1.0000000000000002
   User3 - 相似度: 1.0000000000000002
   User4 - 相似度: 0.8660254037844388
   User6 - 相似度: 0.6666666666666669
   User1 - 相似度: 0.5773502691896258
   User5 - 相似度: 0.3333333333333334
Video88 的推荐：
   User5 - 相似度: 0.6666666666666669
   User4 - 相似度: 0.5773502691896258
   User2 - 相似度: 0.3333333333333334
   User3 - 相似度: 0.3333333333333334
   User6 - 相似度: 0.3333333333333334
   User1 - 相似度: 0.0
Video89 的推荐：
   User2 - 相似度: 1.0000000000000002
   User3 - 相似度: 1.0000000000000002
   User4 - 相似度: 0.8660254037844388
   User6 - 相似度: 0.6666666666666669
   User1 - 相似度: 0.5773502691896258
   User5 - 相似度: 0.3333333333333334
Video90 的推荐：
   User5 - 相似度: 0.816496580927726
   User6 - 相似度: 0.816496580927726
   User4 - 相似度: 0.7071067811865475
   User2 - 相似度: 0.408248290463863
   User3 - 相似度: 0.408248290463863
   User1 - 相似度: 0.0
Video91 的推荐：
   User4 - 相似度: 0.75
   User2 - 相似度: 0.5773502691896258
   User3 - 相似度: 0.5773502691896258
   User5 - 相似度: 0.5773502691896258
   User6 - 相似度: 0.5773502691896258
   User1 - 相似度: 0.5
Video92 的推荐：
   User4 - 相似度: 0.7071067811865475
   User2 - 相似度: 0.408248290463863`
   User3 - 相似度: 0.408248290463863
   User5 - 相似度: 0.408248290463863
   User6 - 相似度: 0.408248290463863
   User1 - 相似度: 0.0
Video93 的推荐：
   User5 - 相似度: 1.0000000000000002
   User6 - 相似度: 0.6666666666666669
   User4 - 相似度: 0.5773502691896258
   User2 - 相似度: 0.3333333333333334
   User3 - 相似度: 0.3333333333333334
   User1 - 相似度: 0.0
Video94 的推荐：
   User2 - 相似度: 0.816496580927726
   User3 - 相似度: 0.816496580927726
   User4 - 相似度: 0.7071067811865475
   User5 - 相似度: 0.408248290463863
   User6 - 相似度: 0.408248290463863
   User1 - 相似度: 0.0
Video95 的推荐：
   User2 - 相似度: 0.816496580927726
   User3 - 相似度: 0.816496580927726
   User1 - 相似度: 0.7071067811865475
   User4 - 相似度: 0.7071067811865475
   User6 - 相似度: 0.408248290463863
   User5 - 相似度: 0.0
Video96 的推荐：
   User5 - 相似度: 1.0000000000000002
   User6 - 相似度: 0.6666666666666669
   User4 - 相似度: 0.5773502691896258
   User2 - 相似度: 0.3333333333333334
   User3 - 相似度: 0.3333333333333334
   User1 - 相似度: 0.0
Video97 的推荐：
   User2 - 相似度: 0.6666666666666669
   User3 - 相似度: 0.6666666666666669
   User1 - 相似度: 0.5773502691896258
   User4 - 相似度: 0.5773502691896258
   User5 - 相似度: 0.3333333333333334
   User6 - 相似度: 0.3333333333333334
Video98 的推荐：
   User5 - 相似度: 0.816496580927726
   User6 - 相似度: 0.408248290463863
   User4 - 相似度: 0.35355339059327373
   User1 - 相似度: 0.0
   User2 - 相似度: 0.0
   User3 - 相似度: 0.0
Video99 的推荐：
   User1 - 相似度: 0.7071067811865475
   User2 - 相似度: 0.408248290463863
   User3 - 相似度: 0.408248290463863
   User5 - 相似度: 0.408248290463863
   User6 - 相似度: 0.408248290463863
   User4 - 相似度: 0.35355339059327373
Video100 的推荐：
   User5 - 相似度: 0.816496580927726
   User6 - 相似度: 0.816496580927726
   User4 - 相似度: 0.7071067811865475
   User2 - 相似度: 0.408248290463863
   User3 - 相似度: 0.408248290463863
   User1 - 相似度: 0.0


```

