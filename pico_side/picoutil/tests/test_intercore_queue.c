void test_interqueue(void)
{
    void print_queue(interqueue_t q)
    {
        void print_node(interqueue_node_t n, size_t i)
        {
            printf("Node %zu: ", i);
            if (n.used)
                printf("used, data = %p, content = %d\n", n.data, *(int*)n.data);
            else
                printf("unused, data = %p, content = <undefined>\n", n.data);
        }
        for (size_t i = 0; i < q.node_count; ++i)
            print_node(q.nodes[i], i);
    }

    interqueue_t q = { 0 };
    interqueue_init(&q, sizeof(int), 2);
    hard_assert(interqueue_empty(&q));
    hard_assert(!interqueue_full(&q));
    print_queue(q);
    picoutil_log(LOG_DEBUG, "Interqueue test part 1 passed");

    
    int a = 1;
    int b = 2;
    interqueue_enqueue(&q, &a);
    hard_assert(!interqueue_empty(&q));
    hard_assert(!interqueue_full(&q));
    print_queue(q);
    picoutil_log(LOG_DEBUG, "Interqueue test part 2 passed");

    interqueue_enqueue(&q, &b);
    hard_assert(!interqueue_empty(&q));
    hard_assert(interqueue_full(&q));
    print_queue(q);
    picoutil_log(LOG_DEBUG, "Interqueue test part 3 passed");

    int c = 0;
    interqueue_dequeue(&q, &c);
    hard_assert(c == 1);
    hard_assert(!interqueue_empty(&q));
    hard_assert(!interqueue_full(&q));
    print_queue(q);
    picoutil_log(LOG_DEBUG, "Interqueue test part 4 passed");

    interqueue_dequeue(&q, &c);
    hard_assert(c == 2);
    hard_assert(interqueue_empty(&q));
    hard_assert(!interqueue_full(&q));
    print_queue(q);
    picoutil_log(LOG_DEBUG, "Interqueue test part 5 passed");

    interqueue_enqueue(&q, &a);
    interqueue_enqueue(&q, &b);
    interqueue_enqueue(&q, &a);
    interqueue_enqueue(&q, &b);
    hard_assert(!interqueue_empty(&q));
    hard_assert(interqueue_full(&q));
    hard_assert(q.node_count == 4);
    print_queue(q);
    picoutil_log(LOG_DEBUG, "Interqueue test part 6 passed");

    interqueue_enqueue(&q, &a);
    hard_assert(q.node_count == 8);
    print_queue(q);
    picoutil_log(LOG_DEBUG, "Interqueue test part 7 passed");

    interqueue_free(&q);
    picoutil_log(LOG_DEBUG, "Interqueue test passed");
}
