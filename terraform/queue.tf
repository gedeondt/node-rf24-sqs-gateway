resource "aws_sqs_queue" "domotics" {
  name   = "nrf4-gateway-queue"
}

resource "aws_sqs_queue_policy" "domotics_gateway" {
  queue_url = aws_sqs_queue.domotics.id

  policy = <<POLICY
{
  "Version": "2012-10-17",
  "Id": "sqspolicy",
  "Statement": [
    {
      "Sid": "First",
      "Effect": "Allow",
      "Principal": {
        "AWS": "${aws_iam_user.gateway.arn}"
      },
      "Action": [
         "sqs:SendMessage",
         "sqs:ReceiveMessage"
      ],
      "Resource": "${aws_sqs_queue.domotics.arn}"
    }
  ]
}
POLICY
}