LOGAN
=====

Simple log parser/viewer/analyzer by running a custom regular expression against the log file. 

The regular expression must include capture groups because based on it, is how all visualization and analysis is done.

## The parsers

All the functionality is built around the concept of __parsers__. A Parser is conformed by:

- A regular expression pattern
- A list of labels for the grid header

## Example

My log file is created by Yii2 PHP Framework with the following format:

```
2023-12-04 11:18:21 [172.18.0.1][35][77547b7eccf3548b7ef1c0ff744f2edd][error][yii\web\HttpException:404] yii\base\InvalidRouteException: Unable to resolve the request "img/loading-plugin.gif". in /app/vendor/yiisoft/yii2/base/Module.php:561
... (the stack trace) ...
2023-12-04 11:18:21 [172.18.0.1][35][77547b7eccf3548b7ef1c0ff744f2edd][info][application] $_GET = []
... (the stack trace) ...
```

And the regular expression that I'm using to parse it is `^(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}) [(.*?)][(.*?)][(.*?)][(info|error|warn)][(.*?)](.*)$` which creates 7 capture groups:
- Date Time
- IP
- User ID
- Request ID
- Level
- Category
- Message

![image](https://github.com/glpzzz/logan/assets/13596248/a6dd3af4-cc02-4686-87fb-c3ee5bd0802f)

**Note:** is there are lines between 2 entries, those will be appended to the content of the last capture group, which is currently asummed as the message.

# This is a WIP

- [ ] Parsers management (add custom)
- [ ] Filtering
- [ ] Charting
