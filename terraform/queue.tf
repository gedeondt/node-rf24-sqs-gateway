resource "aws_sqs_queue" "domotics" {
  name   = "nrf4-gateway-queue"
}

data "aws_iam_policy_document" "domotics_queque_policy_document" {
  statement {
    actions = ["sqs:ReceiveMessage","sqs:SendMessage","sqs:DeleteMessage"]
    effect  = "Allow"
    resources = [aws_sqs_queue.domotics.arn]
  }
}

resource "aws_iam_policy" "domotics_queque_policy" {
  name   = "domotics_queque_policy"
  policy = data.aws_iam_policy_document.domotics_queque_policy_document.json
}

resource "aws_iam_role" "domotics_role" {
   name = "domotics_role"
   managed_policy_arns = [aws_iam_policy.domotics_queque_policy.arn]
   assume_role_policy = <<EOF
{
  "Version": "2012-10-17",
  "Statement": [
    {
      "Action": "sts:AssumeRole",
      "Principal": {
        "AWS": "${aws_iam_role.credentials_lambda_role.arn}"
      },
      "Effect": "Allow",
      "Sid": ""
    }
  ]
}
EOF

}